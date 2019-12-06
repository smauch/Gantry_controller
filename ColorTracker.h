#ifndef COLOR_TRACKER_H
#define COLOR_TRACKER_H

class ColorTracker {
    private:
        int h = 0;
        int s = 0; 
        int v = 0;

        int tolerance = 0;

    public:
        ColorTracker(int iH, int iS, int iV);
        ColorTracker();
