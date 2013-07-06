#include <iostream>
#include <vector>

#include "render.h"

#include "material.h"

inline int next_id(std::vector<int> &value, const int maxID) {
	int k = 0;
	for (;;) {
		value[k]++;
		if (value[k] == maxID)
			value[k] = 0;
		else
			return 0;
		k++;
		if (k == value.size())
			return 1;
	}
	return 0;
}

int main(int argc, char **argv) {
	std::cout << "Path tracing precomputer" << std::endl << std::endl;

	// 640x480の画像、(2x2) * 4 sample / pixel
	edupt::render(640, 480, 64 * 4, 16);
	
	return 0;
	
	/*
	FILE *fp = fopen("image_diffuse.rel", "rb");
	if (fp != NULL) {
		int width;
		fread(&width, sizeof(int), 1, fp);
		int height;
		fread(&height, sizeof(int), 1, fp);
		int NumMaterial;
		int MaxVertices;
		fread(&NumMaterial, sizeof(int), 1, fp);
		fread(&MaxVertices, sizeof(int), 1, fp);
		int spp;
		fread(&spp, sizeof(int), 1, fp);

		const int size = (int)pow((double)NumMaterial, MaxVertices);
		int *relight_weight = new int[size * width * height];

		fread(relight_weight, sizeof(int) * size * width * height, 1, fp);

		edupt::Color *image = new edupt::Color[width * height];
		
		edupt::Color color_table[] = {
			edupt::Color(0.75, 0.25, 0.25),
			edupt::Color(0.25, 0.25, 0.75),
			edupt::Color(0.75, 0.75, 0.75),
			edupt::Color(0.0, 0.0, 0.0),
			edupt::Color(0.75, 0.75, 0.75),
		};
		
		edupt::Color light_table[] = {
			edupt::Color(0.0, 0.0, 0.0),
			edupt::Color(0.0, 0.0, 0.0),
			edupt::Color(0.0, 0.0, 0.0),
			edupt::Color(1.0, 1.0, 1.0),
			edupt::Color(0.0, 0.0, 0.0),
		};
		

		const int precomputed_size = (int)pow((double)NumMaterial, MaxVertices);
		edupt::Color *precomputed = new edupt::Color[precomputed_size];
		
		std::vector<int> now_materialID(MaxVertices);
		for (;;) {
			int idx = 0;
			int w = 1;
			for (int i = 0; i < MaxVertices; ++i) {
				idx += now_materialID[i] * w;
				w *= NumMaterial;
			}

			edupt::Color radiance;
			for (int i = MaxVertices - 1; i >= 0; --i) {
				radiance = light_table[now_materialID[i]] + multiply(color_table[now_materialID[i]], radiance);
			}
			precomputed[idx] = radiance;

			if (next_id(now_materialID, NumMaterial))
				break;
		}


		std::cout << "begin" << std::endl;
#pragma omp parallel for schedule(dynamic, 1) num_threads(10)
		for (int y = 0; y < height; y ++) {
//			std::cerr << "Rendering (y = " << y << ") " << (100.0 * y / (height - 1)) << "%" << std::endl;
			for (int x = 0; x < width; x ++) {
				const int image_index = (height - y - 1) * width + x;

				std::vector<int> now_materialID(MaxVertices);

				for (;;) {
					int idx = 0;
					int w = 1;
					for (int i = 0; i < MaxVertices; ++i) {
						idx += now_materialID[i] * w;
						w *= NumMaterial;
					}
					const double weight = (double)relight_weight[image_index * size + idx] / spp;
					
					if (weight > 0.0) {
						image[image_index] = image[image_index] + weight * precomputed[idx];
					}

					if (next_id(now_materialID, NumMaterial))
						break;

				}
			}
		}
		std::cout << "end" << std::endl;
		// 出力
		edupt::save_ppm_file(std::string("relgithed.ppm"), image, width, height);


		delete[] precomputed;
		delete[] relight_weight;
		fclose(fp);
	}
	*/
}
