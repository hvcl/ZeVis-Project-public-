#include "CellSubsetExplorer.h"
#include "Source/som/SOM.h"
#include "Source/som/ClassesSOM.h"

#include "Source/controller/node/BlockWidget.h"

#include "Source/rf/decision-trees.hxx"
#include "Source/rf/marray.hxx"

CellSubsetExplorer::CellSubsetExplorer(QWidget *parent)
	: QWidget(parent)
{

	cell_layout = new QGridLayout;

	reduction_plot = new View2DPlot(this);


	for (int i = 0; i < 64; i++) {
		ViewCellGLWidget *tcell = new ViewCellGLWidget();
		transferFunction *ttf = new transferFunction();
		connect(tcell, SIGNAL(WhiteListAdded(unsigned int)), this, SLOT(WhiteListAdded(unsigned int)));
		connect(tcell, SIGNAL(BlackListAdded(unsigned int)), this, SLOT(BlackListAdded(unsigned int)));
		connect(tcell, SIGNAL(WhiteListRemoved(unsigned int)), this, SLOT(WhiteListRemoved(unsigned int)));
		connect(tcell, SIGNAL(BlackListRemoved(unsigned int)), this, SLOT(BlackListRemoved(unsigned int)));


		connect(tcell, SIGNAL(TypeUpdated()), reduction_plot, SLOT(update()));
		connect(tcell, SIGNAL(TypeUpdated()), this, SLOT(doRandomForest()));


		connect(ttf, SIGNAL(edited()), tcell, SLOT(update()));
		tcell->tf = ttf;
		tcell->scale = 0.2;
		tcell->background_color = QVector4D(1, 1, 1, 1);
		cell_layout->addWidget(tcell, i / 8, i % 8);

		cell_objects.push_back(tcell);
		tf_objects.push_back(ttf);
	}


	xaxis_combobox = new QComboBox;
	xaxis_combobox->addItem("volume");
	xaxis_combobox->addItem("sphericity");
	xaxis_combobox->addItem("eccentricity");
	xaxis_combobox->addItem("intensity");
	xaxis_combobox->addItem("surfacearea");
	xaxis_combobox->addItem("position_x");
	xaxis_combobox->addItem("position_y");
	xaxis_combobox->addItem("position_z");



	yaxis_combobox = new QComboBox;
	yaxis_combobox->addItem("volume");
	yaxis_combobox->addItem("sphericity");
	yaxis_combobox->addItem("eccentricity");
	yaxis_combobox->addItem("intensity");
	yaxis_combobox->addItem("surfacearea");
	yaxis_combobox->addItem("position_x");
	yaxis_combobox->addItem("position_y");
	yaxis_combobox->addItem("position_z");

	connect(xaxis_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(interacted(int)));
	connect(yaxis_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(interacted(int)));

	QLabel *xaxis_label = new QLabel("x axis: ");
	QLabel *yaxis_label = new QLabel("y axis: ");

	QHBoxLayout *axis_layout = new QHBoxLayout;
	axis_layout->addWidget(xaxis_label);
	axis_layout->addWidget(xaxis_combobox);
	axis_layout->addSpacing(30);
	axis_layout->addWidget(yaxis_label);
	axis_layout->addWidget(yaxis_combobox);

	QWidget *filler = new QWidget;
	filler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout *control_layout = new QVBoxLayout;
	control_layout->addLayout(axis_layout);
	control_layout->addWidget(reduction_plot);
	control_layout->addWidget(filler);

	control_layout->setAlignment(Qt::AlignTop);

	QSplitter *total_layout = new QSplitter;


	QWidget *cell_layout_widget = new QWidget;
	cell_layout_widget->setLayout(cell_layout);

	QWidget *control_layout_widget = new QWidget;
	control_layout_widget->setLayout(control_layout);

	total_layout->addWidget(cell_layout_widget);
	total_layout->addWidget(control_layout_widget);

	QList<int> sizes;
	sizes << 800 << 200;
	total_layout->setSizes(sizes);


	QHBoxLayout *total_layout2 = new QHBoxLayout;
	total_layout2->addWidget(total_layout);

	setLayout(total_layout2);


	worker *cellUpdate = new worker(this);
	cellUpdate->moveToThread(&workerThread);
	connect(&workerThread, &QThread::finished, cellUpdate, &QObject::deleteLater);

	connect(this, SIGNAL(cell_Update()), cellUpdate, SLOT(generateRepresentativeCells()));

	workerThread.start();

}

void CellSubsetExplorer::doRandomForest() {

	qDebug() << "Random forest in";
	if (white_list->size() < 1 || black_list->size() < 1) {
		for (int i = 0; i < n; i++)(*IsForegroundByRF)[i] = true;
		cellIncludeCheck();
		return;
	}
	qDebug() << "Random forest start";

	const size_t numberOfSamples = white_list->size() + black_list->size();
	const size_t numberOfFeatures = 5;

	std::default_random_engine RandomNumberGenerator;
	std::uniform_real_distribution<float> randomDistribution(0.0, 1.0);
	const size_t shape[] = { numberOfSamples, numberOfFeatures };
	andres::Marray<float> features(shape, shape + 2);
	andres::Marray<unsigned char> labels(shape, shape + 1);

	int cnt = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < white_list->size(); j++) {
			if ((*white_list)[j] == input_vector[i]) {
				for (int k = 0; k < 5; k++) {
					features(cnt, k) = inputData[i * 8 + k];
					labels(cnt) = 1;
				}
				cnt++;
				break;
			}
		}
		for (int j = 0; j < black_list->size(); j++) {
			if ((*black_list)[j] == input_vector[i]) {
				for (int k = 0; k < 5; k++) {
					features(cnt, k) = inputData[i * 8 + k];
					labels(cnt) = 0;
				}
				cnt++;
				break;
			}
		}
	}

	andres::ml::DecisionForest<float,unsigned char,float> decisionForest;
	const size_t numberOfDecisionTrees = 50;
	decisionForest.learn(features, labels, numberOfDecisionTrees);

	qDebug() << "Random forest trained";


	const size_t shape2[] = { n, numberOfFeatures };
	andres::Marray<float> features2(shape2, shape2 + 2);
	for (int i = 0; i < n; i++) {
		for (int k = 0; k < 5; k++) {
			features2(i, k) = inputData[i * 8 + k];
		}
	}
	andres::Marray<float> probabilities(shape2, shape2 + 2);
	decisionForest.predict(features2, probabilities);
	
	for (int i = 0; i < n; i++) {
//		qDebug() << probabilities(i);

		if (probabilities(i) > 0.5) {
			(*IsForegroundByRF)[i] = false;
		}
		else {
			(*IsForegroundByRF)[i] = true;
		}
	}
	qDebug() << "Random forest end";

	cellIncludeCheck();
	reduction_plot->update();

}

void CellSubsetExplorer::WhiteListAdded(unsigned int label_value) {
	if (white_list == NULL)return;
	white_list->push_back(label_value);
}
void CellSubsetExplorer::BlackListAdded(unsigned int label_value) {
	if (black_list == NULL)return;
	black_list->push_back(label_value);
}
void CellSubsetExplorer::WhiteListRemoved(unsigned int label_value) {
	if (white_list == NULL)return;
	for (int i = 0; i < white_list->size(); i++) {
		if (label_value == (*white_list)[i]) {
			white_list->remove(i);
			break;
		}
	}
}
void CellSubsetExplorer::BlackListRemoved(unsigned int label_value) {
	if (black_list == NULL)return;
	for (int i = 0; i < black_list->size(); i++) {
		if (label_value == (*black_list)[i]) {
			black_list->remove(i);
			break;
		}
	}
}

void CellSubsetExplorer::interacted(int a) {
	int prev_xaxis = *xaxis;
	int prev_yaxis = *yaxis;

	*xaxis = xaxis_combobox->currentIndex();
	*yaxis = yaxis_combobox->currentIndex();

	if (prev_xaxis != *xaxis || prev_yaxis != *yaxis) {
		if (selectPoints != NULL) {
			if (is_selection == false)noUpdate = true;
			selectPoints->clear();
			*is_selection = false;
			cellIncludeCheck();
		}
	}

	reduction_plot->update();
}
CellSubsetExplorer::~CellSubsetExplorer()
{
	IsNewJob = true;

	workerThread.quit();
	workerThread.wait();
}


bool CellSubsetExplorer::onSegment(QVector2D p, QVector2D q, QVector2D r)
{
	if (q.x() <= max(p.x(), r.x()) && q.x() >= min(p.x(), r.x()) &&
		q.y() <= max(p.y(), r.y()) && q.y() >= min(p.y(), r.y()))
		return true;
	return false;
}

int CellSubsetExplorer::orientation(QVector2D p, QVector2D q, QVector2D r)
{
	float val = (q.y() - p.y()) * (r.x() - q.x()) -
		(q.x() - p.x()) * (r.y() - q.y());

	if (abs(val)<0.0001) return 0;  
	return (val > 0) ? 1 : 2;  
}

bool CellSubsetExplorer::doIntersect(QVector2D p1, QVector2D q1, QVector2D p2, QVector2D q2)
{
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if (o1 != o2 && o3 != o4)
		return true;

	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false;
}

bool CellSubsetExplorer::isInside(QVector2D p)
{
	if (selectPoints->size() < 3)return true;
	QVector2D extreme = QVector2D(3,p.y());
	int count = 0;
	for(int i=0;i< selectPoints->size();i++){
		int next = (i + 1) % selectPoints->size();
		if (doIntersect((*selectPoints)[i], (*selectPoints)[next], p, extreme))
		{
			if (orientation((*selectPoints)[i], p, (*selectPoints)[next]) == 0)
				return onSegment((*selectPoints)[i], p, (*selectPoints)[next]);

			count++;
		}
	}
	if (count % 2 == 0)return false;
	else return true;
}


void CellSubsetExplorer::cellIncludeCheck() {
	if (is_selection == NULL)return;
	if (IsCellIncluded == NULL)return;

	output_cell_list->clear();
	if (*is_selection == false) {
		std::list<unsigned int>::iterator iter = cell_list->begin();
		int cnt = 0;
		for (; iter != cell_list->end(); ++iter) {
			if ((*IsForegroundByRF)[cnt]){
				IsCellIncluded[cnt] = true;
				output_cell_list->push_back(*iter);
			}
			else {
				IsCellIncluded[cnt] = false;
			}
			cnt++;
		}
	}
	else {
		std::list<unsigned int>::iterator iter = cell_list->begin();
		int cnt = 0;
		for (; iter != cell_list->end(); ++iter) {
			bool included = false;

			QVector2D p;


			p.setX(inputData[cnt * 8 + *xaxis]);
			p.setY(inputData[cnt * 8 + *yaxis]);

			included = isInside(p);
			

			IsCellIncluded[cnt] = included & (*IsForegroundByRF)[cnt];
			if (included) {
				output_cell_list->push_back(*iter);
			}
			cnt++;
		}
	}

	reduction_plot->update();
	if (noUpdate == true) {
		noUpdate = false;
	}
	else curBlock->checkNextBlock();


}

worker::worker(CellSubsetExplorer *parent) {
	mWidget = parent;
}

void worker::generateRepresentativeCells() {

	mWidget->IsNewJob = false;


	if (mWidget->inputData != NULL) {
		delete[]mWidget->inputData;
		mWidget->inputData = NULL;
	}
	if (mWidget->input_vector != NULL) {
		delete[]mWidget->input_vector;
		mWidget->input_vector = NULL;
	}
	if (mWidget->IsCellIncluded != NULL) {
		delete[]mWidget->IsCellIncluded;
		mWidget->IsCellIncluded = NULL;
	}
//	if (mWidget->IsForegroundByRF != NULL) {
//		delete[]mWidget->IsForegroundByRF;
//		mWidget->IsForegroundByRF = NULL;
//	}



	mWidget->cell_list = &mWidget->curBlock->CellIndexListInput;
	mWidget->output_cell_list = &mWidget->curBlock->CellIndexListOutput;

	mWidget->IsForegroundByRF = &mWidget->curBlock->IsForegroundByRF;

	mWidget->white_list = &mWidget->curBlock->white_list;
	mWidget->black_list = &mWidget->curBlock->black_list;
	mWidget->selected_list = &mWidget->curBlock->selected_list;

	mWidget->is_selection = &mWidget->curBlock->is_selection;
	mWidget->selectPoints = &mWidget->curBlock->selectPoints;

	mWidget->xaxis = &mWidget->curBlock->xaxis;
	mWidget->yaxis = &mWidget->curBlock->yaxis;


	disconnect(mWidget->xaxis_combobox, SIGNAL(currentIndexChanged(int)), mWidget, SLOT(interacted(int)));
	disconnect(mWidget->yaxis_combobox, SIGNAL(currentIndexChanged(int)), mWidget, SLOT(interacted(int)));

	mWidget->xaxis_combobox->setCurrentIndex(*mWidget->xaxis);
	mWidget->yaxis_combobox->setCurrentIndex(*mWidget->yaxis);

	connect(mWidget->xaxis_combobox, SIGNAL(currentIndexChanged(int)), mWidget, SLOT(interacted(int)));
	connect(mWidget->yaxis_combobox, SIGNAL(currentIndexChanged(int)), mWidget, SLOT(interacted(int)));


	mWidget->w = mWidget->curBlock->w;
	mWidget->h = mWidget->curBlock->h;
	mWidget->d = mWidget->curBlock->d;
	for (int i = 0; i < 64; i++) {
		mGlobals.mainWindow->mContents->Cell_Explorer->volume[i] = &mWidget->curBlock->volume[i];
		mGlobals.mainWindow->mContents->Cell_Explorer->label[i] = &mWidget->curBlock->label[i];
	}

	mGlobals.mainWindow->mContents->contents_center_tabwidget->setCurrentIndex(5);





	qDebug() << "worker start";

	int start_k = 0;

	for (int k = 0; k < 64; k++) {
		mWidget->cell_objects[k]->isVolumeInit = false;
		mWidget->cell_objects[k]->selectType = 0;
	}

	mWidget->n = mWidget->cell_list->size();

//	qDebug() << "debug1";


	mWidget->inputData = new float[mWidget->n * 8]();

//	qDebug() << "debug2";


	int volumeIndex = 0;
	int sphericityIndex = 0;
	int eccentricityIndex = 0;
	int intensityIndex = 0;
	int surfaceareaIndex = 0;

	int xIndex = 0;
	int yIndex = 0;
	int zIndex = 0;



	for (int i = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->featureStartIndex; i < mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->featureEndIndex; i++) {
		if (mGlobals.CurrentProject->mFeature[i].FeatureName == "volume") {
			volumeIndex = i;
		}
		if (mGlobals.CurrentProject->mFeature[i].FeatureName == "sphericity") {
			sphericityIndex = i;
		}
		if (mGlobals.CurrentProject->mFeature[i].FeatureName == "eccentricity") {
			eccentricityIndex = i;
		}
		if (mGlobals.CurrentProject->mFeature[i].FeatureName == "intensity") {
			intensityIndex = i;
		}
		if (mGlobals.CurrentProject->mFeature[i].FeatureName == "surfacearea") {
			surfaceareaIndex = i;
		}
		if (mGlobals.CurrentProject->mFeature[i].FeatureName == "position_x") {
			xIndex = i;
		}
		if (mGlobals.CurrentProject->mFeature[i].FeatureName == "position_y") {
			yIndex = i;
		}
		if (mGlobals.CurrentProject->mFeature[i].FeatureName == "position_z") {
			zIndex = i;
		}
	}
	qDebug() << volumeIndex << sphericityIndex << intensityIndex << surfaceareaIndex << xIndex << yIndex << zIndex;
	qDebug() << "debug3";

	std::list<unsigned int>::iterator iter = mWidget->cell_list->begin();
	mWidget->input_vector = new unsigned int[mWidget->n]();

//	Sleep(3000);

	int cnt = 0;
	for (; iter != mWidget->cell_list->end(); ++iter) {
		mWidget->input_vector[cnt] = *iter;
		cnt++;
	}
	qDebug() << cnt;

	qDebug() << "debug4";

	for (int i = 0; i < 8; i++) {
		mWidget->max_values[i] = -1000;
		mWidget->min_values[i] = 10000000;
	}
	qDebug() << "debug4-1";
	qDebug() << mWidget->n;

	for (int i = 0; i < mWidget->n;i++) {

		mWidget->inputData[i * 8 + 0] = mGlobals.CurrentProject->mFeature[volumeIndex].FeatureMap[mWidget->input_vector[i]];
		mWidget->inputData[i * 8 + 1] = mGlobals.CurrentProject->mFeature[sphericityIndex].FeatureMap[mWidget->input_vector[i]];
		mWidget->inputData[i * 8 + 2] = mGlobals.CurrentProject->mFeature[eccentricityIndex].FeatureMap[mWidget->input_vector[i]];
		mWidget->inputData[i * 8 + 3] = mGlobals.CurrentProject->mFeature[intensityIndex].FeatureMap[mWidget->input_vector[i]];
		mWidget->inputData[i * 8 + 4] = mGlobals.CurrentProject->mFeature[surfaceareaIndex].FeatureMap[mWidget->input_vector[i]];
		mWidget->inputData[i * 8 + 5] = mGlobals.CurrentProject->mFeature[xIndex].FeatureMap[mWidget->input_vector[i]];
		mWidget->inputData[i * 8 + 6] = mGlobals.CurrentProject->mFeature[yIndex].FeatureMap[mWidget->input_vector[i]];
		mWidget->inputData[i * 8 + 7] = mGlobals.CurrentProject->mFeature[zIndex].FeatureMap[mWidget->input_vector[i]];

	}
	qDebug() << "debug4-2";

	for (int i = 0; i < mWidget->n; i++) {
		for (int j = 0; j < 8; j++) {
			if (mWidget->max_values[j] < mWidget->inputData[i * 8 + j])mWidget->max_values[j] = mWidget->inputData[i * 8 + j];
			if (mWidget->min_values[j] > mWidget->inputData[i * 8 + j])mWidget->min_values[j] = mWidget->inputData[i * 8 + j];
		}
	}
	qDebug() << "debug4-3";

	for (int i = 0; i < mWidget->n; i++) {
		for (int j = 0; j < 8; j++) {
			mWidget->inputData[i * 8 + j]=(mWidget->inputData[i * 8 + j]- mWidget->min_values[j])/(mWidget->max_values[j]- mWidget->min_values[j]);
		}
	}

	qDebug() << "input data generated";



	mWidget->IsCellIncluded = new bool[mWidget->n]();
	if (mWidget->selected_list->size() != 0)mWidget->noUpdate = true;

	mWidget->cellIncludeCheck();

	qDebug() << "cell include check init";



	if (mWidget->selected_list->size() == 64) {
		for (int i = 0; i < 64; i++) {
			mWidget->cell_objects[i]->setVolumeData(mWidget->w[i], mWidget->h[i], mWidget->d[i], *mWidget->volume[i], *mWidget->label[i], (*mWidget->selected_list)[i]);

			for (int j = 0; j < mWidget->white_list->size(); j++) {
				if (mWidget->cell_objects[i]->label_value == (*mWidget->white_list)[j])
					mWidget->cell_objects[i]->selectType = 1;
			}
			for (int j = 0; j < mWidget->black_list->size(); j++) {
				if (mWidget->cell_objects[i]->label_value == (*mWidget->black_list)[j])
					mWidget->cell_objects[i]->selectType = 2;
			}
			mWidget->cell_objects[i]->update();

		}
		mWidget->reduction_plot->update();
		return;
	}

	qDebug() << "data generate start";


	if (mWidget->selected_list->size() == 0) {

		qDebug() << "Kmeans start";
		if (group != NULL)delete[] group;
		group = new int[mWidget->n];

		float new_means[64][5];
		int count[64];
		for (int i = 0; i < 64; i++) {
			int ind = float(rand()) / RAND_MAX * mWidget->n;
			for (int j = 0; j < 5; j++) {
				mWidget->curBlock->means[i][j] = mWidget->inputData[ind * 8 + j];
			}
		}
		for (int it = 0; it < 20; it++) {

			int flag = 0;

			for (int i = 0; i < mWidget->n; i++) {

				double min_dis = 9999;
				int min = -1;

				for (int j = 0; j < 64; j++) {
					double dis = 0;
					for (int l = 0; l < 5; l++)
						dis += abs(mWidget->inputData[i * 8 + l] - mWidget->curBlock->means[j][l]);

					if (dis < min_dis) {
						min_dis = dis;
						min = j;
					}
				}

				group[i] = min;

			}


			for (int i = 0; i < 64; i++) {
				count[i] = 0;
				for (int j = 0; j < 5; j++)
					new_means[i][j] = 0;
			}


			for (int i = 0; i < mWidget->n; i++) {
				count[group[i]]++;
				for (int j = 0; j < 5; j++)
					new_means[group[i]][j] += mWidget->inputData[i * 8 + j];

			}



			for (int i = 0; i < 64; i++) {
				for (int j = 0; j < 5; j++) {
					new_means[i][j] /= count[i];
					if (fabs(new_means[i][j] - mWidget->curBlock->means[i][j]) > 0.0001) {
						flag++;
						mWidget->curBlock->means[i][j] = new_means[i][j];
					}
				}
			}

			if (flag == 0)
				break;
		}
	}
	qDebug() << "means";


	for (int i = 0; i < mWidget->selected_list->size(); i++) {
		mWidget->cell_objects[i]->setVolumeData(mWidget->w[i], mWidget->h[i], mWidget->d[i], *mWidget->volume[i], *mWidget->label[i], (*mWidget->selected_list)[i]);
		for (int j = 0; j < mWidget->white_list->size(); j++) {
			if (mWidget->cell_objects[i]->label_value == (*mWidget->white_list)[j])
				mWidget->cell_objects[i]->selectType = 1;
		}
		for (int j = 0; j < mWidget->black_list->size(); j++) {
			if (mWidget->cell_objects[i]->label_value == (*mWidget->black_list)[j])
				mWidget->cell_objects[i]->selectType = 2;
		}
		mWidget->cell_objects[i]->update();
	}
	mWidget->reduction_plot->update();

	start_k = mWidget->selected_list->size();

	//qDebug() << "som start";

	//SOM graphs;
	//graphs.SetData(inputData, n, 5);
	//graphs.InitializeMap(8, 8);

	//graphs.SigmaNeighbouringInitial = 1.5;
	//graphs.SigmaNeighbourhoodFinal = 1;
	//graphs.LearningRateInitial = 0.5;
	//graphs.LearningRateFinal = 0.1;
	//graphs.Train(1000);
	//qDebug() << "som train finish";



	float means[64][5];
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 5; j++) {
			means[i][j] = mWidget->curBlock->means[i][j];
		}
	}

	char fileName[200];
	unsigned char *volume_block = new unsigned char[512 * 512];
	unsigned int *label_block = new unsigned int[512 * 512];

	qDebug() << "main start";


	for (int k = start_k; k < 64; k++) {
		if (mWidget->IsNewJob) {
			break;
		}
//		float minDis = 10000;
		int ind = 0;
		int curx = k % 8;
		int cury = k / 8;

		int random_ind = float(rand()) / RAND_MAX* mWidget->n;
		float min_dis=100000;
		for (int i = 0; i < mWidget->n; i++) {
			float dis = 0;
			for (int j = 0; j < 5; j++) {
				dis += abs(means[k][j] - mWidget->inputData[i * 8 + j]);
			}
			if (dis < min_dis) {
				min_dis = dis;
				ind = i;
			}
		}

		ind = mWidget->input_vector[ind];
		//		int cnt = 0;

		//std::list<unsigned int>::iterator iter = input->begin();
		//for (; iter != input->end(); ++iter) {
		//	cnt++;
		//	if (cnt == random_ind) {
		//		ind = *iter;
		//		break;
		//	}

		//	/*
		//	float dis = (graphs.SOMMap(curx, cury)(0) - mGlobals.CurrentProject->mFeature[volumeIndex].FeatureMap[*iter])
		//		+ (graphs.SOMMap(curx, cury)(1) - mGlobals.CurrentProject->mFeature[sphericityIndex].FeatureMap[*iter])
		//		+ (graphs.SOMMap(curx, cury)(2) - mGlobals.CurrentProject->mFeature[eccentricityIndex].FeatureMap[*iter])
		//		+ (graphs.SOMMap(curx, cury)(3) - mGlobals.CurrentProject->mFeature[intensityIndex].FeatureMap[*iter])
		//		+ (graphs.SOMMap(curx, cury)(4) - mGlobals.CurrentProject->mFeature[surfaceareaIndex].FeatureMap[*iter]);

		//	if (minDis > dis) {
		//		minDis = dis;
		//		ind = *iter;
		//	}
		//	*/
		//}



		qDebug() << curx<<cury<<ind;



		int listIndex = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellIndex[ind];
		cell curCell = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->mCellList[listIndex];


		int xmin = curCell.minbox.x;
		int xmax = curCell.maxbox.x;
		int ymin = curCell.minbox.y;
		int ymax = curCell.maxbox.y;
		int zmin = curCell.minbox.z;
		int zmax = curCell.maxbox.z;
		xmin = (xmin < 4) ? 0 : xmin - 4;
		ymin = (ymin < 4) ? 0 : ymin - 4;
		zmin = (zmin < 4) ? 0 : zmin - 4;
		xmax = xmax + 4;
		ymax = ymax + 4;
		zmax = zmax + 4;
		while ((xmax - xmin) % 4 != 0 && xmin != 0) {
			xmin--;
		}
		while ((xmax - xmin) % 4 != 0) {
			xmax++;
		}

		int w = xmax - xmin;
		int h = ymax - ymin;
		int d = zmax - zmin;

		int startXInd = xmin / 512;
		int endXInd = (xmax - 1) / 512;
		int startYInd = ymin / 512;
		int endYInd = (ymax - 1) / 512;
		int startZInd = zmin / 512;
		int endZInd = (zmax - 1) / 512;

		std::string labelPath = mGlobals.CurrentProject->mLayerCell[mGlobals.CurrentProject->curLayerCell]->DataPathXY;
		std::string emPath = mGlobals.CurrentProject->mLayerBack->DataPathXY;
		qDebug() << "init1";
		if (*mWidget->volume[k] != NULL)delete[] * mWidget->volume[k];
		if (*mWidget->label[k] != NULL)delete[] * mWidget->label[k];
		*mWidget->volume[k] = new unsigned char[w*h*d]();
		*mWidget->label[k] = new unsigned int[w*h*d]();
		qDebug() << "init2";

		for (int iz = startZInd; iz <= endZInd; iz++) {
			for (int iy = startYInd; iy <= endYInd; iy++) {
				for (int ix = startXInd; ix <= endXInd; ix++) {
					int startXCoord = xmin - ix * 512;
					if (startXCoord < 0)startXCoord = 0;
					int startYCoord = ymin - iy * 512;
					if (startYCoord < 0)startYCoord = 0;
					int startZCoord = zmin - iz * 512;
					if (startZCoord < 0)startZCoord = 0;

					int endXCoord = xmax - ix * 512;
					if (endXCoord > 512)endXCoord = 512;
					int endYCoord = ymax - iy * 512;
					if (endYCoord > 512)endYCoord = 512;
					int endZCoord = zmax - iz * 512;
					if (endZCoord > 512)endZCoord = 512;

					sprintf(fileName, "%s%d_%d_%d_0.block", emPath.c_str(), iz, iy, ix);

					std::ifstream f;
					f.open(fileName, std::ifstream::binary);
					//FILE *f = fopen(fileName, "rb");
					//fread(volume_block, 1, 512 * 512 * 512, f);
					//fclose(f);


					sprintf(fileName, "%s%d_%d_%d_0.block", labelPath.c_str(), iz, iy, ix);

					std::ifstream f2;
					f2.open(fileName, std::ifstream::binary);
//					f = fopen(fileName, "rb");
					if (f2.is_open()) {
//						fread(label_block, 4, 512 * 512 * 512, f);
//						fclose(f);
					}

					for (int bz = startZCoord; bz < endZCoord; bz++) {
						long long seekvalue = bz * 512 * 512;
						f.seekg(seekvalue);
						f2.seekg(seekvalue*4);
						f.read((char *)&volume_block[0], 512*512);
						f2.read((char *)&label_block[0], 4*512 * 512);

						for (int by = startYCoord; by < endYCoord; by++) {
							for (int bx = startXCoord; bx < endXCoord; bx++) {
								int global_coord = by * 512 + bx;
								int local_coord = (bz + iz * 512 - zmin)*w*h + (by + iy * 512 - ymin)*w + (bx + ix * 512 - xmin);

								(*mWidget->volume[k])[local_coord] = volume_block[global_coord];
								(*mWidget->label[k])[local_coord] = label_block[global_coord];
							}
						}
					}
					f.close();
					f2.close();

				}
			}
		}
		mWidget->w[k] = w;
		mWidget->h[k] = h;
		mWidget->d[k] = d;
		qDebug() << "init3";
		if (mWidget->IsNewJob) {
			break;
		}
		mWidget->selected_list->push_back(ind);
		mWidget->cell_objects[k]->setVolumeData(mWidget->w[k], mWidget->h[k],mWidget->d[k], *mWidget->volume[k], *mWidget->label[k], (*mWidget->selected_list)[k]);
		mWidget->cell_objects[k]->update();
		qDebug() << "init4";

	}
	delete[]volume_block;
	delete[]label_block;

}
