#ifndef TILE_H
#define TILE_H
#include "gameobject.h"
#include "pixelPainter.h"

class Tile  :   public GameObject
{
    public:
        enum State{
            dead,
            alive
        };
        Tile(Vector2u coord);

        void addNeighbour(Tile *neighbour);
        void setNeighbourList(const vector<Tile*> &list);

        const Vector2u &getCoord() const;
        const State    &getState() const;
        const Color    &getAliveColor() const;

        void prepareForNextMove();
        void tick(const Vector2i &direction);
        void draw(PixelDisplay &display);

        void setAlive();
        void setDead();
    protected:

    private:
        void mixColors();
        void fadeColor(Color &c,float factor);

        Color m_aliveColor;
        Color m_deadColor;
        Vector2u m_coord;

        State m_state;
        State m_nextTickState;

        vector<Tile*> m_neighbourList;
        vector<Color> m_colorList;
};

#endif // TILE_H
