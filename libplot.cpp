//
// Created by shinku on 5/7/19.
//

#include "libplot.hpp"


PlotWindow::PlotWindow(int width, int height) {
    w_height = height;
    w_width = width;
    maxv_x = new double[width];
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return;
    window = SDL_CreateWindow("LibPlot",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN);
    if(window == NULL) return;
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) return;
    if(TTF_Init() < 0) return;
    font = TTF_OpenFont("ipag.ttf",12);
    if(font == NULL) return;
    initialized = true;
}

bool PlotWindow::isReady() {
    return initialized;
}
void PlotWindow::SetGrid(double x_min_, double x_max_, double y_min_, double y_max_, double x_grid_, double y_grid_,
                         GridMode x_gridmode_,GridMode y_gridmode_) {
    x_max = x_max_;
    x_min = x_min_;
    y_max = y_max_;
    y_min = y_min_;
    x_grid = x_grid_;
    y_grid = y_grid_;
    x_gridmode = x_gridmode_;
    y_gridmode = y_gridmode_;
}

int PlotWindow::normGX(double x) {
    if(x_gridmode == GridMode::Logarithm) {
        return 30 + (w_width - margin) * ((log(x)-log(x_min))/(log(x_max)-log(x_min)));
    }
    return 30 + (w_width - margin) * ((x-x_min)/(x_max-x_min));
}

int PlotWindow::normGY(double y) {
    if(y_gridmode == GridMode::Logarithm) {
        return w_height - ((w_height - margin) * ((log(y)-log(y_min))/(log(y_max)-log(y_min))) + margin);
    }
    return w_height - ((w_height - margin) * ((y-y_min)/(y_max-y_min)) + margin);
}

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 1)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

int PlotWindow::HandleEvent() {
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if(ev.type == SDL_QUIT) {
            return 1;
        }
    }
    return 0;
}

void PlotWindow::drawString(std::string str,int x,int y) {
    if(textureMap.find(str) == textureMap.end()) {
        SDL_Color color;
        color.b = 0xff;
        color.g = 0;
        color.r = 0xff;
        color.a = 0xff;
        SDL_Surface* stext = TTF_RenderText_Solid(font,str.c_str(),color);
        textureMap[str] = std::make_pair(SDL_CreateTextureFromSurface(renderer,stext),std::make_pair(stext->w,stext->h));
    }
    auto a = &textureMap[str];
    SDL_Rect r;
    r.w = a->second.first;
    r.h = a->second.second;
    r.x = x;
    r.y = y;
    SDL_RenderCopy(renderer,a->first,NULL,&r);
}

void PlotWindow::DrawLineGraph(std::vector<std::pair<double, double>> sample) {
    sort(sample.begin(),sample.end());
    SDL_SetRenderDrawColor(renderer,0xff,0xff,0xff,0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer,0xA0,0xA0,0xA0,0xff);
    SDL_RenderDrawLine(renderer,0,w_height-margin,w_width,w_height-margin);
    SDL_RenderDrawLine(renderer,margin,0,margin,w_height);
    SDL_SetRenderDrawColor(renderer,0xc0,0xc0,0xc0,0xff);
    if(x_grid != 0 && x_gridmode == GridMode::Logarithm) {
        int count = 0;
        double xg = x_grid;
        for(double i = 0;i < x_max;i += xg) {
            SDL_RenderDrawLine(renderer,normGX(i),0,normGX(i),w_height - margin);
            if(i >= 1000)
                drawString(to_string_with_precision(i/1000.0,1)+"k",normGX(i),w_height-margin + (count % 2)*(margin/2));
            else if(i >= 100)
                drawString(to_string_with_precision(i,0),normGX(i),w_height-margin + (count % 2)*(margin/2));
            else
                drawString(to_string_with_precision(i,1),normGX(i),w_height-margin + (count % 2)*(margin/2));
            count++;
            if(count == 11) {
                count = 2;
                xg = xg * 10;
            }
        }
    }
    if(y_grid != 0 && y_gridmode == GridMode::Logarithm) {
        int count = 0;
        double yg = y_grid;
        for(double i = 0;i < y_max;i += yg) {
            if(normGY(i) < w_height - margin && normGY(i) > 0)  {
            SDL_RenderDrawLine(renderer,margin,normGY(i),w_width,normGY(i));
            if(i >= 1000000)
                drawString(to_string_with_precision(i/1000000.0,1)+"M",0,normGY(i));
            else if(i >= 1000)
                drawString(to_string_with_precision(i/1000.0,1)+"k",0,normGY(i));
            else if(i >= 100)
                drawString(to_string_with_precision(i,0),0,normGY(i));
            else
                drawString(to_string_with_precision(i,1),0,normGY(i));
            }
            count++;
            if(count == 11) {
                count = 2;
                yg = yg * 10;
            }
        }
    }

    SDL_SetRenderDrawColor(renderer,0x00,0x00,0xff,0xff);
    for(int i = 0;i < w_width;i++) maxv_x[i] = INFINITY;

    for(int i = 0;i < sample.size();i++) {
        if(sample[i].first < x_min || sample[i].first > x_max) continue;
        maxv_x[normGX(sample[i].first)] = std::min(maxv_x[normGX(sample[i].first)],sample[i].second);
    }
    int cur = margin,next = margin;
    while(next < w_width) {
        do{
            next++;
        } while(next < w_width && maxv_x[next] == INFINITY);
        SDL_RenderDrawLine(renderer,cur,normGY(maxv_x[cur]),next,normGY(maxv_x[next]));
        cur = next;
    }

    double smin = INFINITY,smax = -INFINITY;
    for(int i = 0;i < w_width;i++) {
        if(maxv_x[i] == INFINITY) continue;
        smin = std::min(smin,maxv_x[i]);
        smax = std::max(smax,maxv_x[i]);
    }
    if(ss_y_max == INFINITY && y_min == y_max) ss_y_max = smax;
    if(ss_y_min == INFINITY && y_min == y_max) ss_y_min = smin;
    if(ss_y_max > smax)
    	ss_y_max = (ss_y_max * 0.995 + smax * 0.005);
    else
    	ss_y_max = (ss_y_max * 0.95 + smax * 0.05);
    if(ss_y_min < smin)
        ss_y_min = (ss_y_min * 0.995 + smin * 0.005);
    else
        ss_y_min = (ss_y_min * 0.95 + smin * 0.05);
    if(ss_y_min != INFINITY) y_min = ss_y_min / 2;
    if(ss_y_max != INFINITY) y_max = ss_y_max * 2;

    SDL_RenderPresent(renderer);
}

PlotWindow::~PlotWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    delete maxv_x;
}
