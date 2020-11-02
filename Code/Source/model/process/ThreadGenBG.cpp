#include "ThreadGenBG.h"

ThreadGenBG::ThreadGenBG() {
	
}
ThreadGenBG::ThreadGenBG(QObject *parent)
	: QThread(parent)
{
	connect(parent, SIGNAL(thread_kill()), this, SLOT(kill_thread()));
}

ThreadGenBG::~ThreadGenBG()
{
}


void ThreadGenBG::crop(unsigned char *src, unsigned char *dst, int x, int y, int rect_width, int rect_height, int src_width, int src_height) {

	for (int i = y; i < y + rect_height; ++i) {
		for (int j = x; j < x + rect_width; ++j) {
			long long dst_index = (i - y)*rect_width + (j - x);
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

void ThreadGenBG::rescale(unsigned char *src, unsigned char *dst, int factor, int width, int height) {
	int dst_height = height / factor;
	int dst_width = width / factor;
	for (int y = 0; y < dst_height; ++y) {
		for (int x = 0; x < dst_width; ++x) {
			
			long long dst_index = y * dst_width + x;
			long long src_index = (y * factor) * width + (x * factor);

			if(src_index < width*height && dst_index < dst_height*dst_width){
				dst[dst_index] = src[src_index];
			}
		}
	}
}

void ThreadGenBG::setVolumeSize(int ax, int ay, int az) {
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
void ThreadGenBG::setPathInfo(std::string save_path, std::string data_type, std::string input_path){
	InputPath = input_path;
	SavePath = save_path;
	DataType = data_type;

	QDir imageSeqDir(QString::fromStdString(InputPath));
	imageSeqDir.setNameFilters(QStringList() << "*.png" << "*.tif" << "*.jpg");
	ImageList = imageSeqDir.entryList();
	ImageList.sort();

}

void ThreadGenBG::multiResolutionXZ() {
	emit progress_generate_bg_log("Generate Multi-Resolution tile XZ");
	float progress_value = 0;
	int x_count;
	int y_count;
	int z_count;
	Utils mUtil;
	mUtil.CalcXYZCount(&x_count, &y_count, &z_count, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);
	omp_set_num_threads(1);

	#pragma omp parallel
	{
		int threadnum = omp_get_thread_num();
		int threadsize = omp_get_num_threads();
		
		for (int y = threadnum; y < whole_volume_y; y += threadsize) {
			if (threadnum == 0) {
				progress_value = ((float)y / (float)whole_volume_y) * 100.0f;
				emit progress_generate_bg(progress_value);
			}
			unsigned char *Image = new unsigned char[whole_volume_x * whole_volume_z];

			//Read Image 
			for (int z = 0; z < z_count; ++z) {
				for (int x = 0; x < x_count; ++x) {
					unsigned char *Tile = new unsigned char[sub_volume_size * sub_volume_size];
					std::string zerolevelTilePath = SavePath + "/XZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + ".tile";
					FILE *tile_rfp = fopen(zerolevelTilePath.c_str(), "rb");
					fread(Tile, sub_volume_size * sub_volume_size, 1, tile_rfp);
					fclose(tile_rfp);

					for (int i = 0; i < sub_volume_size; ++i) {
						for (int j = 0; j < sub_volume_size; ++j) {
							Image[(z*sub_volume_size + i)*whole_volume_z + (x*sub_volume_size + j)] = Tile[i*sub_volume_size + j];
						}
					}
				}
			}
			qDebug() << "Read image done";
			int width = whole_volume_x;
			int height = whole_volume_z;
			for (int lv = 1; lv <= sub_volume_level; ++lv) {
				qDebug() << threadnum << lv << "start";
				unsigned char *resize_image = new unsigned char[width / pow(2, lv) * height / pow(2, lv)];
				rescale(Image, resize_image, pow(2, lv), width, height);

				int x_count_sub;
				int y_count_sub;
				int z_count_sub;
				Utils mUtil;
				mUtil.CalcXYZCount(&x_count_sub, &y_count_sub, &z_count_sub, lv, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);
				qDebug() << threadnum << lv << "resize done";

				for (int x = 0; x < x_count_sub; ++x) {
					for (int z = 0; z < z_count_sub; ++z) {
						unsigned char *cropped = new unsigned char[sub_volume_size * sub_volume_size];
						qDebug() << x << z<< "crop s";
						crop(resize_image, cropped, x*sub_volume_size, z*sub_volume_size, sub_volume_size, sub_volume_size, width / pow(2, lv), height / pow(2, lv));
						qDebug() << x << z << "crop d";

						//XZ write step
						std::string tile_pathXZ = SavePath + "/XZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(lv) + ".tile";
						FILE *xz_tile_fp = fopen(tile_pathXZ.c_str(), "wb");
						fwrite(cropped, sub_volume_size*sub_volume_size, 1, xz_tile_fp);
						fclose(xz_tile_fp);

						delete[] cropped;
					}
				}
				delete[] resize_image;
				qDebug() << threadnum << lv << "crop done";

			}
			//delete[] Image;
			qDebug() << "Finish";


		}
	}
}


void ThreadGenBG::multiResolution(int axis) {
	float progress_value = 0;
	Utils mUtil;
	int x_count_zero, y_count_zero, z_count_zero;
	mUtil.CalcXYZCount(&x_count_zero, &y_count_zero, &z_count_zero, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);
	int BlockDepthCount;
	int x_count_src, y_count_src, z_count_src;
	int x_count_dst, y_count_dst, z_count_dst;
	long long lvalue = 1;
	string src_path = "";
	string dst_path_root = "";
	switch (axis) {
		case 0:
			emit progress_generate_bg_log("Generate Multi-Resolution tile XY");
			mUtil.CalcXYZCount(&x_count_src, &y_count_src, &z_count_src, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);			
			src_path = SavePath + "/XY_" + to_string(0) + ".tile";
			dst_path_root = SavePath + "/XY_";
			BlockDepthCount = whole_volume_z;
			break;
		case 1:
			emit progress_generate_bg_log("Generate Multi-Resolution tile YZ");
			mUtil.CalcXYZCount(&x_count_src, &y_count_src, &z_count_src, 0, whole_volume_z, whole_volume_y, whole_volume_x, sub_volume_size);
			src_path = SavePath + "/YZ_" + to_string(0) + ".tile";
			dst_path_root = SavePath + "/YZ_";
			BlockDepthCount = whole_volume_x;

			break;
		case 2:
			emit progress_generate_bg_log("Generate Multi-Resolution tile ZX");
			mUtil.CalcXYZCount(&x_count_src, &y_count_src, &z_count_src, 0, whole_volume_x, whole_volume_z, whole_volume_y, sub_volume_size);
			src_path = SavePath + "/ZX_" + to_string(0) + ".tile";
			dst_path_root = SavePath + "/ZX_";
			BlockDepthCount = whole_volume_y;
			break;
	}

	FILE *src_fp = fopen(src_path.c_str(), "rb");
	
	
	
	
	for (int b = 0; b < BlockDepthCount; ++b) {

		long long SrcBlockArrSize = lvalue * sub_volume_size *sub_volume_size*sub_volume_size * x_count_src * y_count_src;
		unsigned char *SrcBlockArray = new unsigned char[SrcBlockArrSize];

		fread(SrcBlockArray, SrcBlockArrSize, 1, src_fp);
		

		for (int lv = 1; lv < sub_volume_level; ++lv) {
			string dst_path = dst_path_root + to_string(lv) + ".tile";
			FILE *dst_fp = fopen(dst_path.c_str(), "ab");

			int x_count_dst, y_count_dst, z_count_dst;
			
			mUtil.CalcXYZCount(&x_count_dst, &y_count_dst, &z_count_dst, lv, x_count_src*sub_volume_size, y_count_src*sub_volume_size, sub_volume_size, sub_volume_size);


			long long DstBlockArrSize = lvalue * sub_volume_size * sub_volume_size * sub_volume_size * x_count_dst * y_count_dst;

			unsigned char *DstBlockArray = new unsigned char[DstBlockArrSize];

			#pragma omp parallel
			{
				int threadnum = omp_get_thread_num();
				int threadsize = omp_get_num_threads();

				for (int z = threadnum; z < sub_volume_size; z += threadsize) {


					if (threadnum == 0) {
						progress_value = ((float)b*sub_volume_size + z / (float)BlockDepthCount*sub_volume_size) * 100.0f;
						emit progress_generate_bg(progress_value);
					}
					long long image_width = x_count_src * sub_volume_size;
					long long image_height = y_count_src * sub_volume_size;
					unsigned char *image = new unsigned char[image_width*image_height];

					for (int y = 0; y < y_count_src; ++y) {
						for (int x = 0; x < x_count_src; ++x) {
							long long blocksize = lvalue * sub_volume_size * sub_volume_size * sub_volume_size;
							long long ptr = (blocksize * x_count_src * y) + (blocksize * x) + (lvalue * sub_volume_size * sub_volume_size * (z % sub_volume_size));

							for (int i = 0; i < sub_volume_size; ++i) {
								for (int j = 0; j < sub_volume_size; ++j) {
									long long image_ptr = lvalue * image_width * (y * sub_volume_size + i) + (x * sub_volume_size + j);
									image[image_ptr] = SrcBlockArray[ptr + i * sub_volume_size + j];
								}
							}
						}
					}


					int resize_width = image_width / pow(2, lv);
					int resize_height = image_height / pow(2, lv);
					unsigned char *image_resize = new unsigned char[resize_width * resize_height];
					rescale(image, image_resize, pow(2, lv), image_width, image_height);
					int x_count, y_count, z_count;
					mUtil.CalcXYZCount(&x_count, &y_count, &z_count, lv, image_width, image_height, sub_volume_size, sub_volume_size);


					for (int y = 0; y < y_count; ++y) {
						for (int x = 0; x < x_count; ++x) {
							unsigned char *croppedXY = new unsigned char[sub_volume_size * sub_volume_size];
							crop(image_resize, croppedXY, x*sub_volume_size, y*sub_volume_size, sub_volume_size, sub_volume_size, resize_width, resize_height);
							long long blocksize = lvalue * sub_volume_size * sub_volume_size * sub_volume_size;
							long long ptr = (blocksize * x_count * y) + (blocksize * x) + (lvalue * sub_volume_size * sub_volume_size * (z % sub_volume_size));
							memcpy(&DstBlockArray[ptr], croppedXY, sub_volume_size*sub_volume_size);
							delete[] croppedXY;
						}
					}

					delete[] image_resize;
					delete[] image;
				}
			}

			fwrite(DstBlockArray, DstBlockArrSize, 1, dst_fp);
			delete[] DstBlockArray;
			fclose(dst_fp);
		}
		
	}

}

void ThreadGenBG::multiResolutionYZ() {
	emit progress_generate_bg_log("Generate Multi-Resolution tile YZ");
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
				emit progress_generate_bg(progress_value);
			}
			unsigned char *Image = new unsigned char[whole_volume_y * whole_volume_z];

			//Read Image 
			for (int y = 0; y < y_count; ++y) {
				for (int z = 0; z < z_count; ++z) {
					unsigned char *Tile = new unsigned char[sub_volume_size * sub_volume_size];
					std::string zerolevelTilePath = SavePath + "/YZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + ".tile";
					FILE *tile_rfp = fopen(zerolevelTilePath.c_str(), "rb");
					fread(Tile, sub_volume_size * sub_volume_size, 1, tile_rfp);
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
				qDebug() << threadnum << lv << "start";
				unsigned char *resize_image = new unsigned char[width / pow(2, lv) * height / pow(2, lv)];
				rescale(Image, resize_image, pow(2, lv), width, height);

				int x_count_sub;
				int y_count_sub;
				int z_count_sub;
				Utils mUtil;
				mUtil.CalcXYZCount(&x_count_sub, &y_count_sub, &z_count_sub, lv, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);

				qDebug() << threadnum << lv << "resize done";
				for (int y = 0; y < y_count_sub; ++y) {
					for (int z = 0; z < z_count_sub; ++z) {
						unsigned char *cropped = new unsigned char[sub_volume_size * sub_volume_size];

						crop(resize_image, cropped, z*sub_volume_size, y*sub_volume_size, sub_volume_size, sub_volume_size, width / pow(2, lv), height / pow(2, lv));

						//XY write step
						std::string tile_pathYZ = SavePath + "/YZ/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(lv) + ".tile";
						FILE *yz_tile_fp = fopen(tile_pathYZ.c_str(), "wb");
						fwrite(cropped, sub_volume_size*sub_volume_size, 1, yz_tile_fp);
						fclose(yz_tile_fp);

						delete[] cropped;
					}
				}
				qDebug() << threadnum << lv << "crop done";
				delete[] resize_image;
			}
			delete[] Image;
		}
	}
}

void ThreadGenBG::compositingXZYZ() {
	emit progress_generate_bg_log("Compositing XZ, YZ");
	float progress_value = 0;

	int x_count_lv0;
	int y_count_lv0;
	int z_count_lv0;
	Utils mUtil;
	mUtil.CalcXYZCount(&x_count_lv0, &y_count_lv0, &z_count_lv0, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);
	int z_size = z_count_lv0 * sub_volume_size;

	long long lvalue = 1;
	long long block_size = lvalue * sub_volume_size * sub_volume_size *sub_volume_size;
	long long tile_size = lvalue * sub_volume_size * sub_volume_size;
	
	//Block Read
	for (int cz = 0; cz < z_count_lv0; ++cz) {
		progress_value = ((float)cz / (float)z_count_lv0) * 100.0f;
		emit progress_generate_bg(progress_value);
		for (int y = 0; y < y_count_lv0; ++y) {
			for (int x = 0; x < x_count_lv0; ++x) {
				unsigned char *block = new unsigned char[block_size];
				for (int z = cz * sub_volume_size; z < (cz + 1)*sub_volume_size; ++z) {
					std::string block_tilePath = SavePath + "/XY/" + std::to_string(z) + "_" + std::to_string(y) + "_" + std::to_string(x) + "_" + std::to_string(0) + ".tile";
					FILE *blck_rfp = fopen(block_tilePath.c_str(), "rb");
					fread(&block[(z-cz*sub_volume_size)*tile_size], tile_size, 1, blck_rfp);
					fclose(blck_rfp);
				}
				#pragma omp parallel
				{
					int threadnum = omp_get_thread_num();
					int threadsize = omp_get_num_threads();

					for (int y_sub = y * sub_volume_size + threadnum; y_sub < (y + 1)*sub_volume_size; y_sub+=threadsize) {
						unsigned char *tileXZ = new unsigned char[tile_size];
						for (int i = 0; i < sub_volume_size; ++i) {
							for (int j = 0; j < sub_volume_size; ++j) {
								long long srcIdx = lvalue * i * sub_volume_size * sub_volume_size + (y_sub - y * sub_volume_size) * sub_volume_size + j;
								tileXZ[i*sub_volume_size + j] = block[srcIdx];
							}
						}
						std::string tilepath = SavePath + "/XZ/" + std::to_string(cz) + "_" + std::to_string(y_sub) + "_" + std::to_string(x) + "_" + std::to_string(0) + ".tile";
						FILE *tile_wfp = fopen(tilepath.c_str(), "wb");
						fwrite(tileXZ, sub_volume_size * sub_volume_size, 1, tile_wfp);
						fclose(tile_wfp);
						delete[] tileXZ;
					}
				}

				#pragma omp parallel
				{
					int threadnum = omp_get_thread_num();
					int threadsize = omp_get_num_threads();

					for (int x_sub = x * sub_volume_size + threadnum; x_sub < (x + 1)*sub_volume_size; x_sub += threadsize) {
						unsigned char *tileYZ = new unsigned char[tile_size];
						for (int i = 0; i < sub_volume_size; ++i) {
							for (int j = 0; j < sub_volume_size; ++j) {
								long long srcIdx = lvalue * i * sub_volume_size * sub_volume_size + (j) * sub_volume_size + (x_sub - x * sub_volume_size);
								tileYZ[i*sub_volume_size + j] = block[srcIdx];
							}
						}
						std::string tilepath = SavePath + "/YZ/" + std::to_string(cz) + "_" + std::to_string(y) + "_" + std::to_string(x_sub) + "_" + std::to_string(0) + ".tile";
						FILE *tile_wfp = fopen(tilepath.c_str(), "wb");
						fwrite(tileYZ, sub_volume_size * sub_volume_size, 1, tile_wfp);
						fclose(tile_wfp);
						delete[] tileYZ;
					}
				}
				delete[] block;
			}
		}
	}
}

void ThreadGenBG::kill_thread() {
	this->terminate();
	qDebug() << "in thread kill_thread";
	delete [] sub_volume;
}


void ThreadGenBG::run()
{
	float progress_value = 0;
	float progress_step = 100.0 / whole_volume_z;
	
	emit progress_generate_bg(progress_value);
	
	std::string headerfile = SavePath + "/header.bgl";
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
	omp_set_num_threads(omp_get_max_threads()/2);
	qDebug() << omp_get_num_threads();
	
	long long lvalue = 1;
	string tile_pathXY = SavePath + "/XY_" + to_string(0) + ".tile";
	FILE * xy_tile_fp = fopen(tile_pathXY.c_str(), "wb");

	int x_count;
	int y_count;
	int z_count;
	Utils mUtil;
	mUtil.CalcXYZCount(&x_count, &y_count, &z_count, 0, whole_volume_x, whole_volume_y, whole_volume_z, sub_volume_size);
	long long blockArrSize = lvalue * sub_volume_size *sub_volume_size*sub_volume_size * x_count * y_count;
	unsigned char *BlockArray = new unsigned char[blockArrSize];

	for (int z_block = 0; z_block < z_count; ++z_block) {
		#pragma omp parallel shared (BlockArray)
		{
			int threadnum = omp_get_thread_num();
			int threadsize = omp_get_num_threads();

			for (int z = z_block*sub_volume_size + threadnum; z < z_block*sub_volume_size+sub_volume_size; z += threadsize) {
				QImage *image;
				QString path;
				if (z >= whole_volume_z) {
					image = new QImage(whole_volume_x, whole_volume_y, QImage::Format_Grayscale8);
				}
				else {
					path = QString::fromStdString(InputPath) + "/" + ImageList.at(z);
					image = new QImage();
					image->load(path);
				}

				if (threadnum == 0) {
					progress_value = ((float)z / (float)whole_volume_z) * 100.0f;
					emit progress_generate_bg(progress_value);
					emit progress_generate_bg_log("Generate tile XY, " + path);
				}
				
				int width = image->width();
				int height = image->height();
				
				unsigned char *image_raw = new unsigned char[width * height];
				
				int index = 0;
				for (int y = 0; y < image->height(); ++y) {
					for (int x = 0; x < image->width(); ++x) {
						image_raw[index++] = (int)image->pixel(x, y);
					}
				}
				

				for (int y = 0; y < y_count; ++y) {
					for (int x = 0; x < x_count; ++x) {
						unsigned char *croppedXY = new unsigned char[sub_volume_size * sub_volume_size];
						crop(image_raw, croppedXY, x*sub_volume_size, y*sub_volume_size, sub_volume_size, sub_volume_size, width , height );
						long long blocksize = lvalue * sub_volume_size * sub_volume_size * sub_volume_size;
						long long ptr = (blocksize * x_count * y) + (blocksize * x) + (lvalue * sub_volume_size * sub_volume_size * (z % sub_volume_size));
						memcpy(&BlockArray[ptr], croppedXY, sub_volume_size*sub_volume_size);
						delete[] croppedXY;
					}
				}

				delete[] image_raw;

			}
		}

		fwrite(BlockArray, blockArrSize, 1, xy_tile_fp);

	}
	fclose(xy_tile_fp);
	delete[] BlockArray;

	multiResolution(0);


	//compositingXZYZ();
	//multiResolutionXZ();
	//compositingYZ();
	//multiResolutionYZ();

	emit finish_generate_bg();
}

