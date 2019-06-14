//
// Created by shinku on 5/7/19.
//

#ifndef FFT_WAV_LIBPLOT_HPP
#define FFT_WAV_LIBPLOT_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <utility>

class PlotWindow {
public:
    enum GridMode {
        Linear,Logarithm
    };
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    void drawString(std::string str,int x,int y);
    std::map<std::string,std::pair<SDL_Texture*,std::pair<int,int>>> textureMap;
    int w_width;
    int w_height;
    double* maxv_x;
    bool initialized = false;
    int margin = 30;
    double x_min = 0,x_max = 100,y_min = -1,y_max = 1,x_grid = 0,y_grid = 0;
    double ss_y_min = INFINITY,ss_y_max = INFINITY;
    GridMode x_gridmode = GridMode::Linear,y_gridmode = GridMode::Linear;
    int normGX(double x);
    int normGY(double y);
public:
    PlotWindow() = delete;
    PlotWindow(int width,int height);
    ~PlotWindow();
    bool isReady();
    int HandleEvent();
    void SetGrid(double x_min_,double x_max_,double y_min_,double y_max_,double x_grid_,double y_grid_,GridMode x_gridmode_,GridMode y_gridmode_);
    void DrawLineGraph(std::vector<std::pair<double,double>>& sample);
};

#endif //FFT_WAV_LIBPLOT_HPP
