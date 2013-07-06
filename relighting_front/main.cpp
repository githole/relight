#include <SDL.h>
#include <SDL_syswm.h>
#include <iostream>
#include <vector>

#include <Windows.h>
#include "resource.h"
#include "resource1.h"
#include  <commctrl.h>


#include "vec.h"

static HWND hDialog;
static HWND hWindow;
BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);


// リライティング
Color color_table[] = {
	Color(0.0, 0.0, 0.0),
	Color(0.0, 0.0, 0.0),
	Color(0.0, 0.0, 0.0),
	Color(0.0, 0.0, 0.0),
	Color(0.0, 0.0, 0.0),
};
		
Color light_table[] = {
	Color(0.0, 0.0, 0.0),
	Color(0.0, 0.0, 0.0),
	Color(0.0, 0.0, 0.0),
	Color(0.0, 0.0, 0.0),
	Color(0.0, 0.0, 0.0),
};

// ダイアログプロシージャ
BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
	
	static BITMAPINFO bmpInfo;
	static LPDWORD lpPixel;
	static HBITMAP hBitmap;
	static HDC hMemDC;
	static SDL_Surface* image;
	
	RECT rect = {0, 0, 1000, 1000};
			
	PAINTSTRUCT		ps; 

	static bool preview = false;

	switch( msg ){
	case WM_INITDIALOG:
		{
		}
		return TRUE;
	case WM_TIMER:		
		{
			static int first = 0;
			if (first == 0) {
				for (int id = IDC_SLIDER1; id <= IDC_SLIDER30; ++id) {
					SendMessage(GetDlgItem(hDialog, id), TBM_SETRANGE,(WPARAM)TRUE,MAKELPARAM(0,100)); 
					SendMessage(GetDlgItem(hDialog, id), TBM_SETPOS, true, 0);
				}
				
				first = 1;
			}
		}

		InvalidateRgn(hDlg, NULL, FALSE);
		UpdateWindow(hDlg);

		return 0;
		
	case WM_PAINT:
		{
			BeginPaint(hDlg,&ps);
			
			EndPaint(hDlg,&ps);
		}
		return TRUE;

	case WM_HSCROLL:
		switch(LOWORD(wp)) {

		case TB_THUMBTRACK :
			for (int mat = 0; mat < 5; ++mat) {
				const int idx = IDC_SLIDER1 + mat * 6;
				
				const DWORD mr = SendMessage(GetDlgItem(hDialog, idx  ), TBM_GETPOS, 0, 0);
				const DWORD mg = SendMessage(GetDlgItem(hDialog, idx+1), TBM_GETPOS, 0, 0);
				const DWORD mb = SendMessage(GetDlgItem(hDialog, idx+2), TBM_GETPOS, 0, 0);
				const DWORD lr = SendMessage(GetDlgItem(hDialog, idx+3), TBM_GETPOS, 0, 0);
				const DWORD lg = SendMessage(GetDlgItem(hDialog, idx+4), TBM_GETPOS, 0, 0);
				const DWORD lb = SendMessage(GetDlgItem(hDialog, idx+5), TBM_GETPOS, 0, 0);

				color_table[mat].x_ = mr / 100.0;
				color_table[mat].y_ = mg / 100.0;
				color_table[mat].z_ = mb / 100.0;
				
				light_table[mat].x_ = lr / 3.0;
				light_table[mat].y_ = lg / 3.0;
				light_table[mat].z_ = lb / 3.0;

			}
			break;

		}
		return TRUE;

	case WM_CLOSE:
		ShowWindow( hDialog, SW_HIDE );
		return TRUE;
	}

	return FALSE;
}

int ProcessSDLEvents() {
    SDL_Event eve;
    while (SDL_PollEvent(&eve)) {
        switch(eve.type) {
		case SDL_KEYDOWN:
			/*
			{
			Uint16 ch = eve.key.keysym.unicode;
			SDLKey key = isprint(ch) ? (SDLKey)ch : eve.key.keysym.sym;

			if (eve.key.keysym.sym == SDLK_F5) {
				CompileShader();
			}
			if (eve.key.keysym.sym == SDLK_LEFT)
				myAngle -= 0.01f;
			if (eve.key.keysym.sym == SDLK_RIGHT)
				myAngle += 0.01f;
			
			if (eve.key.keysym.sym == SDLK_a)
				sunAngle -= 0.01f;
			if (eve.key.keysym.sym == SDLK_s)
				sunAngle += 0.01f;

			if (eve.key.keysym.sym == SDLK_ESCAPE)
				return -1;
			}
			*/
			break;

        case SDL_QUIT:
            return -1;
        }
    }
	return 0;
}

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

inline double clamp(double x){ 
	if (x < 0.0)
		return 0.0;
	if (x > 1.0)
		return 1.0;
	return x;
} 

inline int to_int(double x){
	return int(pow(clamp(x), 1/2.2) * 255 + 0.5);
}

int main(int argc, char** argv) {
	int flags = SDL_HWSURFACE;
	const int width = 640;
	const int height = 480;


    const SDL_VideoInfo* info = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << SDL_GetError() << std::endl;
		return -1;
    }
	std::cerr << "SDL_Init succeeded" << std ::endl;
	
	SDL_WM_SetCaption("test", NULL);
	
    info = SDL_GetVideoInfo( );

    if (!info) {
		std::cerr << SDL_GetError() << std::endl;
		return -1;
    }
	std::cerr <<  "SDL_GetVideoInfo succeeded" << std::endl;
	
    const int bpp = info->vfmt->BitsPerPixel;
	
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_EnableUNICODE(true);
	SDL_EnableKeyRepeat(25, 1);
	
    if (SDL_SetVideoMode(width, height, bpp, flags) == 0) {
		std::cerr << SDL_GetError() << std::endl;
		return -1;
    }
	std::cerr << "SDL_SetVideoMode succeeded" << std::endl;

	// Initialize Controll Window
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	SDL_GetWMInfo(&wm_info);
	hWindow = wm_info.window;
	
	hDialog = CreateDialog((HINSTANCE)GetModuleHandle(0), (LPCSTR)IDD_DIALOG1, hWindow, DlgProc);
	ShowWindow(hDialog, SW_SHOW);



	// relighting
	FILE *fp = fopen("I:\\Projects\\relighting\\relighting\\image_spec.rel", "rb");
	if (fp == NULL)
		return -1;

	
	int rl_width;
	fread(&rl_width, sizeof(int), 1, fp);
	int rl_height;
	fread(&rl_height, sizeof(int), 1, fp);
	int NumMaterial;
	int MaxVertices;
	fread(&NumMaterial, sizeof(int), 1, fp);
	fread(&MaxVertices, sizeof(int), 1, fp);
	int spp;
	fread(&spp, sizeof(int), 1, fp);
	
	const int size = (int)pow((double)NumMaterial, MaxVertices);
	int *relight_weight = new int[size * width * height];
	fread(relight_weight, sizeof(int) * size * width * height, 1, fp);
	



	std::vector<int> *path_table = new std::vector<int>[width * height];
	
	for (int y = 0; y < height; y ++) {
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
					path_table[image_index].push_back(idx);
				}

				if (next_id(now_materialID, NumMaterial))
					break;
			}
		}
	}


	Color *image = new Color[width * height];
	

	
	SDL_Surface *surface;
    Uint32 rmask, gmask, bmask, amask;
	
	SDL_Rect rect, scr_rect;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE, rl_width, rl_height, bpp, rmask, gmask, bmask, amask); 
	
    if(surface == NULL) {
		std::cerr << SDL_GetError() << std::endl;
		return -1;
    }
	const int precomputed_size = (int)pow((double)NumMaterial, MaxVertices);
	Color *precomputed = new Color[precomputed_size];

	while (1) {
		if (ProcessSDLEvents() < 0)
			break;
		
		SDL_LockSurface(surface);

		static int count;
		count ++;

		
		int now = SDL_GetTicks();
	
		std::vector<int> now_materialID(MaxVertices);
		for (;;) {
			int idx = 0;
			int w = 1;
			for (int i = 0; i < MaxVertices; ++i) {
				idx += now_materialID[i] * w;
				w *= NumMaterial;
			}

			Color radiance;
			for (int i = MaxVertices - 1; i >= 0; --i) {
				radiance = light_table[now_materialID[i]] + multiply(color_table[now_materialID[i]], radiance);
			}
			precomputed[idx] = radiance;

			if (next_id(now_materialID, NumMaterial))
				break;
		}
	
#pragma omp parallel for schedule(dynamic, 1) num_threads(10)
		for (int y = 0; y < height; y ++) {
			for (int x = 0; x < width; x ++) {
				const int image_index = (height - y - 1) * width + x;
				image[image_index] = Color();
				for (int i = 0; i < path_table[image_index].size(); ++i) {
					const int idx = path_table[image_index][i];
				
					const double weight = (double)relight_weight[image_index * size + idx] / spp;
					image[image_index] = image[image_index] + weight * precomputed[idx];
				}

			}
		}
		//


	
		for (int iy = 0; iy < rl_height; ++iy) { 
			for (int ix = 0; ix < rl_width; ++ix) {
				unsigned char *pixel = (unsigned char*)surface->pixels;
				pixel += (iy * surface->pitch) + (ix * sizeof(unsigned int));

				const int r = to_int(image[iy * rl_width + ix].x_);
				const int g = to_int(image[iy * rl_width + ix].y_);
				const int b = to_int(image[iy * rl_width + ix].z_);

				*((unsigned int*)pixel) = 0xff000000 + ((b << 16) + (g << 8) + r);
			}
		}

		SDL_UnlockSurface(surface);
	
		rect.x = 0;
		rect.y = 0;
		rect.w = surface->w;
		rect.h = surface->h;

		scr_rect.x = 0;
		scr_rect.y = 0;

		SDL_BlitSurface(surface, &rect, SDL_GetVideoSurface(), &scr_rect);
		SDL_Flip(SDL_GetVideoSurface());
		
		int tm = SDL_GetTicks() - now;
		char buf[256];
		sprintf(buf, "%d", tm);
		SDL_WM_SetCaption(buf, NULL);

		SDL_Delay(1);
	}
	
	SDL_Quit();

	return 0;
}
