#include "ThreadGenLB.h"

ThreadGenLB::ThreadGenLB() {

}
ThreadGenLB::ThreadGenLB(QObject *parent)
	: QThread(parent)
{
	connect(parent, SIGNAL(thread_kill()), this, SLOT(kill_thread()));
}

ThreadGenLB::~ThreadGenLB()
{
}


void ThreadGenLB::setVolumeSize(int ax, int ay, int az) {
	whole_volume_x = ax;
	whole_volume_y = ay;
	whole_volume_z = az;

	int temp_z_size = az;

	sub_volume_size = 512;
	int level = 0;

	for (level = 0; sub_volume_size < temp_z_size; temp_z_size /= 2, level++);
	sub_volume_level = level;

	qDebug() << sub_volume_size;
	qDebug() << sub_volume_level;
}

void ThreadGenLB::setPathInfo(std::string save_path, std::string data_type, std::string input_path) {
	InputPath = input_path;
	SavePath = save_path;
	DataType = data_type;

	QDir imageSeqDir(QString::fromStdString(InputPath));
	imageSeqDir.setNameFilters(QStringList() << "*.raw");
	ImageList = imageSeqDir.entryList();
	ImageList.sort();
	qDebug() << "image list settings";
}

void ThreadGenLB::rescale(unsigned int *src, unsigned int *dst, int factor, int width, int height) {
	int dst_height = height / factor;
	int dst_width = width / factor;
		
	for (int y = 0; y < dst_height; ++y) {
		for (int x = 0; x < dst_width; ++x) {
			long long dst_index = y * dst_width + x;
			long long src_index = (y * factor) * width + (x * factor);
			if (src_index < width*height && dst_index < dst_height*dst_width) {
				dst[dst_index] = src[src_index];
			}
			//dst[dst_index] = src[src_index];
		}
	}
}

void ThreadGenLB::crop(unsigned int *src, unsigned int *dst, int x, int y, int rect_width, int rect_height, int src_width, int src_height) {
	
	for (int i = y; i < y+rect_height; ++i) {
		for (int j = x; j < x+rect_width; ++j) {
			long long dst_index = (i-y)*rect_width + (j-x);
			long long src_index = i * src_width + j;
			if (i >= src_height || j >= src_width) {
				dst[dst_index] = 0;
			}
			else {
				dst[dst_index] = src[src_index];
			}
			
		}
	}
}

void ThreadGenLB::multiResolutionXZ() {
	emit progress_generate_lb_log("Generate Multi-Resolution tile XZ");
	float progress_value = 0;
	int x_count;
	int y_count;
	int z_count;
	Utils mUtil;
	mUtil.CalcXYZCount(&x_count, &y_count, &z_count, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);

	#pragma omp parallel
	{
		int threadnum = omp_get_thread_num();
		int threadsize = omp_get_num_threads();

		for (int y = threadnum; y < whole_volume_y; y += threadsize) {
			if (threadnum == 0) {
				progress_value = ((float)y / (float)whole_volume_y) * 100.0f;
				emit progress_generate_lb(progress_value);
			}
			unsigned int *Image = new unsigned int[whole_volume_x * whole_volume_z];

			//Read Image 
			for (int z = 0; z < z_count; ++z) {
				for (int x = 0; x < x_count; ++x) {
					unsigned int *Tile = new unsigned int[sub_volume_size * sub_volume_size];
					std::string zerolevelTilePath = SavePath + "/XZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + ".tile";
					FILE *tile_rfp = fopen(zerolevelTilePath.c_str(), "rb");
					fread(Tile, sizeof(unsigned int)*sub_volume_size * sub_volume_size, 1, tile_rfp);
					fclose(tile_rfp);

					for (int i = 0; i < sub_volume_size; ++i) {
						for (int j = 0; j < sub_volume_size; ++j) {
							Image[(z*sub_volume_size + i)*whole_volume_x + (x*sub_volume_size + j)] = Tile[i*sub_volume_size + j];
						}
					}
				}
			}

			//TEST
			std::string testpath = SavePath + "test.raw";
			FILE *test_fp = fopen(testpath.c_str(), "wb");
			fwrite(Image, sizeof(unsigned int)*whole_volume_x * whole_volume_z, 1, test_fp);
			fclose(test_fp);


			int width = whole_volume_x;
			int height = whole_volume_z;
			for (int lv = 1; lv <= sub_volume_level; ++lv) {
				unsigned int *resize_image = new unsigned int[width / pow(2, lv) * height / pow(2, lv)];
				rescale(Image, resize_image, pow(2, lv), width, height);

				int x_count_sub;
				int y_count_sub;
				int z_count_sub;
				Utils mUtil;
				mUtil.CalcXYZCount(&x_count_sub, &y_count_sub, &z_count_sub, lv, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);

				for (int x = 0; x < x_count_sub; ++x) {
					for (int z = 0; z < z_count_sub; ++z) {
						unsigned int *cropped = new unsigned int[sub_volume_size * sub_volume_size];

						crop(resize_image, cropped, x*sub_volume_size, z*sub_volume_size, sub_volume_size, sub_volume_size, width / pow(2, lv), height / pow(2, lv));

						//XY write step
						std::string tile_pathXZ = SavePath + "/XZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(lv) + ".tile";
						FILE *xz_tile_fp = fopen(tile_pathXZ.c_str(), "wb");
						fwrite(cropped, sizeof(unsigned int)*sub_volume_size*sub_volume_size, 1, xz_tile_fp);
						fclose(xz_tile_fp);

						delete[] cropped;
					}
				}
				delete[] resize_image;
			}
		}
	}
}
void ThreadGenLB::multiResolutionYZ() {
	emit progress_generate_lb_log("Generate Multi-Resolution tile YZ");
	float progress_value = 0;
	int x_count;
	int y_count;
	int z_count;
	Utils mUtil;
	mUtil.CalcXYZCount(&x_count, &y_count, &z_count, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);

	#pragma omp parallel
	{
		int threadnum = omp_get_thread_num();
		int threadsize = omp_get_num_threads();

		for (int x = threadnum; x < whole_volume_x; x += threadsize) {
			if (threadnum == 0) {
				progress_value = ((float)x / (float)whole_volume_x) * 100.0f;
				emit progress_generate_lb(progress_value);
			}
			unsigned int *Image = new unsigned int[whole_volume_y * whole_volume_z];

			//Read Image 
			for (int y = 0; y < y_count; ++y) {
				for (int z = 0; z < z_count; ++z) {
					unsigned int *Tile = new unsigned int[sub_volume_size * sub_volume_size];
					std::string zerolevelTilePath = SavePath + "/YZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + ".tile";
					FILE *tile_rfp = fopen(zerolevelTilePath.c_str(), "rb");
					fread(Tile, sizeof(unsigned int)*sub_volume_size * sub_volume_size, 1, tile_rfp);
					fclose(tile_rfp);

					for (int i = 0; i < sub_volume_size; ++i) {
						for (int j = 0; j < sub_volume_size; ++j) {
							Image[(y*sub_volume_size + i)*whole_volume_z + (z*sub_volume_size + j)] = Tile[i*sub_volume_size + j];
						}
					}
				}
			}

			int width = whole_volume_z;
			int height = whole_volume_y;
			for (int lv = 1; lv <= sub_volume_level; ++lv) {
				unsigned int *resize_image = new unsigned int[width / pow(2, lv) * height / pow(2, lv)];
				rescale(Image, resize_image, pow(2, lv), width, height);

				int x_count_sub;
				int y_count_sub;
				int z_count_sub;
				Utils mUtil;
				mUtil.CalcXYZCount(&x_count_sub, &y_count_sub, &z_count_sub, lv, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);

				for (int y = 0; y < y_count_sub; ++y) {
					for (int z = 0; z < z_count_sub; ++z) {
						unsigned int *cropped = new unsigned int[sub_volume_size * sub_volume_size];

						crop(resize_image, cropped, z*sub_volume_size, y*sub_volume_size, sub_volume_size, sub_volume_size, width / pow(2, lv), height / pow(2, lv));

						//XY write step
						std::string tile_pathYZ = SavePath + "/YZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(lv) + ".tile";
						FILE *yz_tile_fp = fopen(tile_pathYZ.c_str(), "wb");
						fwrite(cropped, sizeof(unsigned int)*sub_volume_size*sub_volume_size, 1, yz_tile_fp);
						fclose(yz_tile_fp);

						delete[] cropped;
					}
				}
				delete[] resize_image;
			}
		}
	}
}

void ThreadGenLB::compositingXZ() {
	emit progress_generate_lb_log("Compositing XZ");
	float progress_value = 0;
	int x_count;
	int y_count;
	int z_count;
	Utils mUtil;
	mUtil.CalcXYZCount(&x_count, &y_count, &z_count, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);


	//XZ Compositing
	#pragma omp parallel
	{
		int threadnum = omp_get_thread_num();
		int threadsize = omp_get_num_threads();



		for (int y = threadnum; y < whole_volume_y; y += threadsize) {
			if (threadnum == 0) {
				progress_value = ((float)y / (float)whole_volume_y) * 100.0f;
				emit progress_generate_lb(progress_value);
			}

			for (int z = 0; z < z_count; ++z) {
				for (int x = 0; x < x_count; ++x) {
					unsigned int *Tile = new unsigned int[sub_volume_size * sub_volume_size];
					int eachsize = sub_volume_size / threadsize + 1;

					for (int tnum = 0; tnum < threadsize; ++tnum) {
						unsigned int *temp_tile = new unsigned int[sub_volume_size * eachsize];
						std::string linedataPath = SavePath + "/XZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + "_" + std::to_string(tnum) + ".linedata";
						//qDebug() << QString::fromStdString(linedataPath);
						FILE *line_rfp = fopen(linedataPath.c_str(), "rb");
						fread(temp_tile, sizeof(unsigned int)*sub_volume_size * eachsize, 1, line_rfp);
						fclose(line_rfp);

						for (int t = 0; t < eachsize; t += 1) {
							for (int line = 0; line < sub_volume_size; ++line) {
								if ((t*threadsize + tnum)*sub_volume_size + line < sub_volume_size*sub_volume_size)
									Tile[(t*threadsize + tnum)*sub_volume_size + line] = temp_tile[sub_volume_size*t + line];
							}
						}
						QFile *rmFile = new QFile(QString::fromStdString(linedataPath));
						rmFile->remove();
						rmFile->close();
						delete[] temp_tile;
					}

					std::string tilepath = SavePath + "/XZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + ".tile";
					FILE *tile_wfp = fopen(tilepath.c_str(), "wb");
					fwrite(Tile, sizeof(unsigned int)*sub_volume_size * sub_volume_size, 1, tile_wfp);
					fclose(tile_wfp);
					delete[] Tile;
				}
			}
		}
	}
}

void ThreadGenLB::compositingYZ() {

	float progress_value = 0;
	int x_count;
	int y_count;
	int z_count;
	Utils mUtil;
	mUtil.CalcXYZCount(&x_count, &y_count, &z_count, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);

	emit progress_generate_lb_log("Compositing YZ");
	//YZ Compositing
	#pragma omp parallel
	{
		int threadnum = omp_get_thread_num();
		int threadsize = omp_get_num_threads();



		for (int x = threadnum; x < whole_volume_x; x += threadsize) {
			if (threadnum == 0) {
				progress_value = ((float)x / (float)whole_volume_x) * 100.0f;
				emit progress_generate_lb(progress_value);
			}

			for (int y = 0; y < y_count; ++y) {
				for (int z = 0; z < z_count; ++z) {
					unsigned int *Tile = new unsigned int[sub_volume_size * sub_volume_size];
					int eachsize = sub_volume_size / threadsize + 1;

					for (int tnum = 0; tnum < threadsize; ++tnum) {
						unsigned int *temp_tile = new unsigned int[sub_volume_size * eachsize];
						std::string linedataPath = SavePath + "/YZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + "_" + std::to_string(tnum) + ".linedata";
						//qDebug() << QString::fromStdString(linedataPath);
						FILE *line_rfp = fopen(linedataPath.c_str(), "rb");
						fread(temp_tile, sizeof(unsigned int)*sub_volume_size * eachsize, 1, line_rfp);
						fclose(line_rfp);

						for (int t = 0; t < eachsize; t += 1) {
							for (int line = 0; line < sub_volume_size; ++line) {
								if ((t*threadsize + tnum) < sub_volume_size)
									Tile[line*sub_volume_size + (t*threadsize + tnum)] = temp_tile[sub_volume_size*t + line];
							}
						}
						QFile *rmFile = new QFile(QString::fromStdString(linedataPath));
						rmFile->remove();
						rmFile->close();
						delete[] temp_tile;
					}

					std::string tilepath = SavePath + "/YZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + ".tile";
					FILE *tile_wfp = fopen(tilepath.c_str(), "wb");
					fwrite(Tile, sizeof(unsigned int)*sub_volume_size * sub_volume_size, 1, tile_wfp);
					fclose(tile_wfp);
					delete[] Tile;
				}
			}
		}
	}
}

void ThreadGenLB::kill_thread() {
	this->terminate();
	qDebug() << "in thread kill_thread";
}


void ThreadGenLB::run()
{
	float progress_value = 0;
	float progress_step = 100.0 / whole_volume_z;

	emit progress_generate_lb(progress_value);

	QDir Dir(QString::fromStdString(SavePath));

	Dir.mkdir("XY");
	Dir.mkdir("YZ");
	Dir.mkdir("XZ");

	//header write
	std::string headerfile = SavePath + "/header.lbl";
	FILE *writebgl = fopen(headerfile.c_str(), "wb");

	string header_x = "#volume x size : " + to_string(whole_volume_x) + "\n";
	string header_y = "#volume y size : " + to_string(whole_volume_y) + "\n";
	string header_z = "#volume z size : " + to_string(whole_volume_z) + "\n";
	string header_size = "#sub-tile size : " + to_string(sub_volume_size) + "\n";
	string header_level = "#data level : " + to_string(sub_volume_level) + "\n";
	string header_type = "#data type : " + DataType + "\n";

	fwrite(header_x.c_str(), header_x.length(), 1, writebgl);
	fwrite(header_y.c_str(), header_y.length(), 1, writebgl);
	fwrite(header_z.c_str(), header_z.length(), 1, writebgl);
	fwrite(header_size.c_str(), header_size.length(), 1, writebgl);
	fwrite(header_level.c_str(), header_level.length(), 1, writebgl);
	fwrite(header_type.c_str(), header_type.length(), 1, writebgl);

	fclose(writebgl);
	ImageList.sort();


	qDebug() << "Thread Max Num";
	qDebug() << omp_get_max_threads();
	omp_set_num_threads(omp_get_max_threads() / 2);
	qDebug() << omp_get_num_threads();
	
	#pragma omp parallel
	{
		int threadnum = omp_get_thread_num();
		int threadsize = omp_get_num_threads();
		for (int z = threadnum; z < whole_volume_z; z += threadsize) {
			QString path = QString::fromStdString(InputPath) + "/" + ImageList.at(z);

			if (threadnum == 0) {
				progress_value = ((float)z / (float)whole_volume_z) * 100.0f;
				emit progress_generate_lb(progress_value);
				emit progress_generate_lb_log("Generate tile XY, " + path);
			}

			int width = whole_volume_x;
			int height = whole_volume_y;
			unsigned int *image = new unsigned int[width * height];
			ifstream img_ifs(path.toStdString(), std::ifstream::binary);
			img_ifs.read((char *)&image[0], width*height * sizeof(unsigned int));


			for (int lv = 0; lv <= sub_volume_level; ++lv) {

				unsigned int *resize_image = new unsigned int[width / pow(2, lv) * height / pow(2, lv)];
				rescale(image, resize_image, pow(2, lv), width, height);


				int x_count;
				int y_count;
				int z_count;
				Utils mUtil;
				mUtil.CalcXYZCount(&x_count, &y_count, &z_count, lv, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);


				for (int y = 0; y < y_count; ++y) {
					for (int x = 0; x < x_count; ++x) {
						unsigned int *croppedXY = new unsigned int[sub_volume_size * sub_volume_size];
				/*		unsigned int *croppedZX_line = new unsigned int[sub_volume_size];
						unsigned int *croppedYZ_line = new unsigned int[sub_volume_size];
*/
		/*				int x1 = x * sub_volume_size * pow(2, lv);
						int x2 = (x + 1) * sub_volume_size * pow(2, lv);
						int y1 = y * sub_volume_size * pow(2, lv);
						int y2 = (y + 1) * sub_volume_size * pow(2, lv);*/
						crop(resize_image, croppedXY, x*sub_volume_size, y*sub_volume_size, sub_volume_size, sub_volume_size, width / pow(2, lv), height / pow(2, lv));

						//XY write step
						std::string tile_pathXY = SavePath + "/XY/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(lv) + ".tile";
						FILE *xy_tile_fp = fopen(tile_pathXY.c_str(), "wb");
						fwrite(croppedXY, sizeof(unsigned int)*sub_volume_size*sub_volume_size, 1, xy_tile_fp);
						fclose(xy_tile_fp);

						//if (lv == 0) {
						//	//XZ write step
						//	for (int zx_y = 0; zx_y < sub_volume_size; ++zx_y) {
						//		int z_idx = z / (int)(sub_volume_size * pow(2, lv));
						//		int z_mod = (z / (int)pow(2, lv)) % sub_volume_size;
						//		std::string tile_pathZX = SavePath + "/XZ/" + std::to_string(z_idx) + "_" + std::to_string(zx_y + y1) + "_" + std::to_string(x) + "_" + std::to_string(lv) + "_" + std::to_string(threadnum) + ".linedata";

						//		FILE *zx_tile_wfp = fopen(tile_pathZX.c_str(), "ab");
						//		for (int zx_x = 0; zx_x < sub_volume_size; ++zx_x) {
						//			croppedZX_line[zx_x] = croppedXY[zx_y*sub_volume_size + zx_x];
						//		}
						//		fwrite(croppedZX_line, sizeof(unsigned int)*sub_volume_size, 1, zx_tile_wfp);
						//		fclose(zx_tile_wfp);
						//	}


						//	//YZ write step
						//	for (int yz_x = 0; yz_x < sub_volume_size; ++yz_x) {
						//		int z_idx = z / (int)(sub_volume_size * pow(2, lv));
						//		int z_mod = (z / (int)pow(2, lv)) % sub_volume_size;
						//		std::string tile_pathYZ = SavePath + "/YZ/" + std::to_string(z_idx) + "_" + std::to_string(y) + "_" + std::to_string(yz_x + x1) + "_" + std::to_string(lv) + "_" + std::to_string(threadnum) + ".linedata";

						//		FILE *yz_tile_wfp = fopen(tile_pathYZ.c_str(), "ab");
						//		for (int yz_y = 0; yz_y < sub_volume_size; ++yz_y) {
						//			croppedYZ_line[yz_y] = croppedXY[yz_y*sub_volume_size + yz_x];
						//		}
						//		fwrite(croppedYZ_line, sizeof(unsigned int)*sub_volume_size, 1, yz_tile_wfp);
						//		fclose(yz_tile_wfp);
						//	}
						//}

						delete[] croppedXY;
						//delete[] croppedZX_line;
						//delete[] croppedYZ_line;
					}
				}
				delete[] resize_image;
			}
			delete[] image;
		}
	}
	//compositingXZ();
	//multiResolutionXZ();
	//compositingYZ();
	//multiResolutionYZ();

	emit finish_generate_lb();
}