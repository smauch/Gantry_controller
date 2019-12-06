#ifndef POLAR_H
#define POLAR_H
    
class Polar {
    private:
        int radius;
        double angle;

    public:
        Polar(double radius, double angle);
        Polar(int x, int y);

        Polar();
            
        int getRadius() { return radius; }

        double getAngle() { return angle; }

        void add(double addedAngle); 

        double calculateStep(Polar nextValue); 

        double getXCartesianCoordinates(); 

        double getYCartesianCoordinates(); 
};

#endif
