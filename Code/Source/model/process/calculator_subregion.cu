#include "calculator_subregion.cuh"


extern "C"
__device__ int _pow_(int a, int b) {
	int value = 1;
	for (int i = 0; i < b; ++i) {
		value *= a;
	}
	return value;
}

extern "C"
__global__ void _cuda_parallel_sum(unsigned char *in, int num_elements, int *sum, const float *information)
{
	//Holds intermediates in shared memory reduction
	__syncthreads();
	__shared__ int buffer[WARP_SIZE];
	int globalIdx = blockIdx.x * blockDim.x + threadIdx.x;
	int tile_size = (int)information[0] * (int)information[0];
	int level_size = _pow_(2, (int)information[11]);
	//information : block_width, tablesize, pos_x,y,z , up_x,y,z , global_x,y,z
	if ((globalIdx / tile_size) % level_size == 0) {

		float vx = -1 * information[2] + (information[8] + ((int)(globalIdx % tile_size) % (int)information[0]));
		float vy = -1 * information[3] + (information[9] + ((int)(globalIdx % tile_size) / (int)information[0]));
		float vz = -1 * information[4] + (information[10] + (globalIdx / tile_size)) / level_size;
		float ux = information[5];
		float uy = information[6];
		float uz = information[7];


		int lane = threadIdx.x % WARP_SIZE;
		int temp;
		while (globalIdx < num_elements)
		{
			// All threads in a block of 1024 take an element

			if (ux * vx + uy * vy + uz * vz >= 0) {
				temp = in[globalIdx] > 0 ? 1 : 0;
			}
			else {
				temp = 0;
			}

			// All warps in this block (32) compute the sum of all
			// threads in their warp
			for (int delta = WARP_SIZE / 2; delta > 0; delta /= 2)
			{
				temp += __shfl_xor(temp, delta);
			}
			// Write all 32 of these partial sums to shared memory
			if (lane == 0)
			{
				buffer[threadIdx.x / WARP_SIZE] = temp;
			}
			__syncthreads();
			// Add the remaining 32 partial sums using a single warp
			if (threadIdx.x < WARP_SIZE)
			{
				temp = buffer[threadIdx.x];
				for (int delta = WARP_SIZE / 2; delta > 0; delta /= 2)
				{
					temp += __shfl_xor(temp, delta);
				}
			}
			if (threadIdx.x == 0)
			{
				atomicAdd(sum, temp);
			}
			// Jump ahead 1024 * #SMs to the next region of numbers to sum
			globalIdx += blockDim.x * gridDim.x;
			__syncthreads();
		}
	}

}

extern "C"
__global__ void _touch_test(const unsigned char * subregion, unsigned int *cell, int *table, const float *information)
{
	// Get our global thread ID
	int id = blockIdx.x*blockDim.x + threadIdx.x;
	int tile_size = (int)information[0] * (int)information[0];
	int level_size = _pow_(2, (int)information[11]);
	//information : block_width, tablesize, pos_x,y,z , up_x,y,z , global_x,y,z
	if ((id / tile_size) % level_size == 0) {

		float vx = -1 * information[2] + (information[8] + ((int)(id % tile_size) % (int)information[0]));
		float vy = -1 * information[3] + (information[9] + ((int)(id % tile_size) / (int)information[0]));
		float vz = -1 * information[4] + (information[10] + (id / tile_size)) / level_size;
		float ux = information[5];
		float uy = information[6];
		float uz = information[7];

		if (ux * vx + uy * vy + uz * vz >= 0) {
			if (subregion[id] > 0) {
				if (cell[id] != 0) {
					table[cell[id]] = 1;
					cell[id] = 0;
				}
			}
		}
	}
}


extern "C"
__global__ void _intersect_test(unsigned int *cell, int *table, int *itable, const float *information)
{
	// Get our global thread ID
	int id = blockIdx.x*blockDim.x + threadIdx.x;
	int tile_size = (int)information[0] * (int)information[0];
	//information : block_width, tablesize, pos_x,y,z , up_x,y,z , global_x,y,z
	int level_size = _pow_(2, (int)information[11]);

	if ((id / tile_size) % level_size == 0) {
		float vx = -1 * information[2] + (information[8] + ((int)(id % tile_size) % (int)information[0]));
		float vy = -1 * information[3] + (information[9] + ((int)(id % tile_size) / (int)information[0]));
		float vz = -1 * information[4] + (information[10] + (id / tile_size)) / level_size;
		float ux = information[5];
		float uy = information[6];
		float uz = information[7];

		if (ux * vx + uy * vy + uz * vz >= 0) {
			if (cell[id] != 0) {
				if (table[cell[id]] == 1) {
					itable[cell[id]] = 1;
				}
			}
		}
	}

	
}

extern "C"
__global__ void _vector_add(int *table, int *itable)
{
	// Get our global thread ID
	int id = blockIdx.x*blockDim.x + threadIdx.x;
	table[id] = table[id] + itable[id];
}



extern "C"
__global__ void _touch_test2(const unsigned char * subregion, unsigned int *cell, int *table, int baseBlockSize)
{

	// Get our global thread ID
	int id = blockIdx.x*blockDim.x + threadIdx.x;

	if (subregion[id] > 0) {
		if (cell[id] != 0) {
			table[cell[id]]=2;
		}
	}
}
extern "C"
__global__ void _intersect_test2(const unsigned char * subregion, unsigned int *cell, int *table, int baseBlockSize)
{

	// Get our global thread ID
	int id = blockIdx.x*blockDim.x + threadIdx.x;

	if (subregion[id] == 0) {
		if (table[cell[id]] == 2) {
			table[cell[id]] = 1;
		}
	}
}


extern "C"
__global__ void _cuda_parallel_type_decision(unsigned char *in, int num_elements, int *sum, const int *information)
{
	//Holds intermediates in shared memory reduction
	__syncthreads();
	__shared__ int buffer[WARP_SIZE];
	int globalIdx = blockIdx.x * blockDim.x + threadIdx.x;
	int tile_size = (int)information[0] * (int)information[0];
	//information : basic block size(512), block size, start pos_x,y,z

	int lane = threadIdx.x % WARP_SIZE;
	int temp;
	while (globalIdx < num_elements)
	{
		// All threads in a block of 1024 take an element
		temp = in[globalIdx] > 0 ? 1 : 0;

		// All warps in this block (32) compute the sum of all
		// threads in their warp
		for (int delta = WARP_SIZE / 2; delta > 0; delta /= 2)
		{
			temp += __shfl_xor(temp, delta);
		}
		// Write all 32 of these partial sums to shared memory
		if (lane == 0)
		{
			buffer[threadIdx.x / WARP_SIZE] = temp;
		}
		__syncthreads();
		// Add the remaining 32 partial sums using a single warp
		if (threadIdx.x < WARP_SIZE)
		{
			temp = buffer[threadIdx.x];
			for (int delta = WARP_SIZE / 2; delta > 0; delta /= 2)
			{
				temp += __shfl_xor(temp, delta);
			}
		}
		if (threadIdx.x == 0)
		{
			atomicAdd(sum, temp);
		}
		// Jump ahead 1024 * #SMs to the next region of numbers to sum
		globalIdx += blockDim.x * gridDim.x;
		__syncthreads();
	}

}


int cuda_parallel_sum(const unsigned char * a, const float *information) {
	// Get device properties to compute optimal launch bounds
	cudaDeviceProp prop;
	cudaGetDeviceProperties(&prop, 0);
	int N = (int)information[0] * (int)information[0] * (int)information[0];
	int num_SMs = prop.multiProcessorCount;
	// pad array with zeros to allow sum algorithm to work
	int batch_size = num_SMs * 1024;
	int padding = (batch_size - (N % batch_size)) % batch_size;
	// b is the new padded array
	unsigned char * b = new unsigned char[N + padding];
	memcpy(b, a, N * sizeof(unsigned char));
	memset(b + N, 0, padding * sizeof(unsigned char));

	// Copy array "b" to GPU 
	unsigned char *d_b;
	cudaMalloc((void**)&d_b, (N + padding) * sizeof(unsigned char));
	cudaMemcpy(d_b, b, (N + padding) * sizeof(unsigned char), cudaMemcpyHostToDevice);

	float *d_information;
	cudaMalloc((void**)&d_information, 12 * sizeof(float));
	cudaMemcpy(d_information, information, 12 * sizeof(float), cudaMemcpyHostToDevice);


	// Result
	int result = 0.0;
	int * d_result;
	cudaMalloc((void**)&d_result, sizeof(int));
	cudaMemcpy(d_result, &result, sizeof(int), cudaMemcpyHostToDevice);

	// Call kernel to get sum
	_cuda_parallel_sum << < num_SMs, 1024 >> >(d_b, N + padding, d_result, d_information);
	// Read in results
	cudaMemcpy(&result, d_result, sizeof(int), cudaMemcpyDeviceToHost);

	// Clean up
	cudaFree(d_result);
	cudaFree(d_b);
	cudaFree(d_information);

	
	free(b);

	return result;
}

int* cuda_parallel_inclusion(const unsigned char * subregion, unsigned int *cell, const float *information) {
	//information : block_width, tablesize, pos_x,y,z , up_x,y,z , global_x,y,z
	int blockSize, gridSize;
	int N = (int)information[0] * (int)information[0] * (int)information[0];
	int t_size = (int)information[1];

	blockSize = 1024;
	gridSize = (int)ceil((float)N / blockSize);

	// Copy array to GPU 
	unsigned char *d_subregion;
	cudaMalloc((void**)&d_subregion, N * sizeof(unsigned char));
	cudaMemcpy(d_subregion, subregion, N * sizeof(unsigned char), cudaMemcpyHostToDevice);

	unsigned int *d_cell;
	cudaMalloc((void**)&d_cell, N * sizeof(unsigned int));
	cudaMemcpy(d_cell, cell, N * sizeof(unsigned int), cudaMemcpyHostToDevice);

	int *table = new int[t_size];
	memset(table, 0, sizeof(int) *t_size);
	int *d_table;
	cudaMalloc((void**)&d_table, t_size * sizeof(int));
	cudaMemcpy(d_table, table, t_size * sizeof(int), cudaMemcpyHostToDevice);

	float *d_information;
	cudaMalloc((void**)&d_information, 12 * sizeof(float));
	cudaMemcpy(d_information, information, 12 * sizeof(float), cudaMemcpyHostToDevice);


	// Call kernel to get sum
	_touch_test <<< gridSize, blockSize >>>(d_subregion, d_cell, d_table, d_information);
	
	
	int *table_i = new int[t_size];
	memset(table_i, 0, sizeof(int) *t_size);
	int *d_itable;
	cudaMalloc((void**)&d_itable, t_size * sizeof(int));
	cudaMemcpy(d_itable, table_i, t_size * sizeof(int), cudaMemcpyHostToDevice);

	_intersect_test <<< gridSize, blockSize >>>(d_cell, d_table, d_itable, d_information);


	blockSize = 1024;
	gridSize = (int)ceil((float)t_size / blockSize);

	_vector_add << < gridSize, blockSize >> > (d_table, d_itable);

	
	cudaMemcpy(table, d_table, t_size * sizeof(int), cudaMemcpyDeviceToHost);
	
	// Clean up
	cudaFree(d_subregion);
	cudaFree(d_cell);
	cudaFree(d_table);
	cudaFree(d_itable);
	cudaFree(d_information);

	return table;
}

int* cuda_parallel_cell_type_decision(const unsigned char * subregion, unsigned int *cell, int baseBlockSize,int cell_number) {
	int blockSize, gridSize;
	int N = baseBlockSize*baseBlockSize*baseBlockSize;
	int t_size = cell_number;

	blockSize = 1024;
	gridSize = (int)ceil((float)N / blockSize);

	// Copy array to GPU 
	unsigned char *d_subregion;
	cudaMalloc((void**)&d_subregion, N * sizeof(unsigned char));
	cudaMemcpy(d_subregion, subregion, N * sizeof(unsigned char), cudaMemcpyHostToDevice);

	unsigned int *d_cell;
	cudaMalloc((void**)&d_cell, N * sizeof(unsigned int));
	cudaMemcpy(d_cell, cell, N * sizeof(unsigned int), cudaMemcpyHostToDevice);

	int *table = new int[t_size];
	memset(table, 0, sizeof(int) *t_size);
	int *d_table;
	cudaMalloc((void**)&d_table, t_size * sizeof(int));
	cudaMemcpy(d_table, table, t_size * sizeof(int), cudaMemcpyHostToDevice);


	// Call kernel to get sum
	_touch_test2 <<< gridSize, blockSize >>>(d_subregion, d_cell, d_table, baseBlockSize);
	_intersect_test2 <<< gridSize, blockSize >>>(d_subregion, d_cell, d_table, baseBlockSize);

	
	
	cudaMemcpy(table, d_table, t_size * sizeof(int), cudaMemcpyDeviceToHost);
	
	// Clean up
	cudaFree(d_subregion);
	cudaFree(d_cell);
	cudaFree(d_table);

	return table;
}


int cuda_parallel_type_decision(const unsigned char * subregion, const int *information) {
	// Get device properties to compute optimal launch bounds
	cudaDeviceProp prop;
	cudaGetDeviceProperties(&prop, 0);
	//information: 0:basic block size, 1:block size, 2:start x, 3:start y, 4: start z
	int N = information[0]*information[0]*information[0];
	int num_SMs = prop.multiProcessorCount;
	// pad array with zeros to allow sum algorithm to work
	int batch_size = num_SMs * 1024;
	int padding = (batch_size - (N % batch_size)) % batch_size;
	// b is the new padded array
	unsigned char * b = new unsigned char[N + padding];
	memcpy(b, subregion, N * sizeof(unsigned char));
	memset(b + N, 0, padding * sizeof(unsigned char));

	// Copy array "b" to GPU 
	unsigned char *d_b;
	cudaMalloc((void**)&d_b, (N + padding) * sizeof(unsigned char));
	cudaMemcpy(d_b, b, (N + padding) * sizeof(unsigned char), cudaMemcpyHostToDevice);

	int *d_information;
	cudaMalloc((void**)&d_information, 5 * sizeof(int));
	cudaMemcpy(d_information, information, 5 * sizeof(int), cudaMemcpyHostToDevice);


	// Result
	int result = 0.0;
	int * d_result;
	cudaMalloc((void**)&d_result, sizeof(int));
	cudaMemcpy(d_result, &result, sizeof(int), cudaMemcpyHostToDevice);

	// Call kernel to get sum
	_cuda_parallel_type_decision << < num_SMs, 1024 >> >(d_b, N + padding, d_result, d_information);
	// Read in results
	cudaMemcpy(&result, d_result, sizeof(int), cudaMemcpyDeviceToHost);

	// Clean up
	cudaFree(d_result);
	cudaFree(d_b);
	free(b);

	if(result==0)return 0;
	else if(result==information[1]*information[1]*information[1])return 2;
	else return 1;
}
