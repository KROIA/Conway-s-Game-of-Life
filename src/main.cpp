#include "pixelengine.h"
#include "tile.h"

#include "QDebug"

const string resourcePath = "..\\..\\resources"; // Where your images are stored
const Vector2u mapsize(100,100);    // Mapsize for the game
float colorFadeFactor = 0.8; // Factor how fast a dead cell will go to the color black. 1-> never, 0->instent

PixelEngine *engine;
Event keyEvent_P;
Event keyEvent_SPCAE;
Event keyEvent_C;
Event keyEvent_1;
Event keyEvent_2;
Event keyEvent_ENTER;
vector<vector<Tile*>    > map;
RectF mapFrame;
DisplayText *displayText_pause;
DisplayText *displayText_startInfo;


bool tickPause = true;
bool leftMouseKilckActive = false;
bool rightMouseKilckActive = false;

GameObjectGroup *generateMap(Vector2u size);

void clearMap();
void setTileAlive(Vector2i mousePos);
void setTileDead(Vector2i mousePos);
Vector2u getTilePos(Vector2i mousePos,bool &outOfArea);


// These functions will later be called from the engine
// userEventLoop: Here you can handle your Events (KeyEvents).
void userEventLoop(float tickInterval,unsigned long long tick,const vector<sf::Event> &eventList);
void userDisplayLoop(float tickInterval,unsigned long long tick,PixelDisplay &display);

void loadFromImage(const string &image,Vector2u drawPos);

int main(int argc, char *argv[])
{
#ifdef BUILD_WITH_EASY_PROFILER
    EASY_PROFILER_ENABLE;
    EASY_MAIN_THREAD;
#endif

    Vector2u mapsize(100,100);
    engine = new PixelEngine(mapsize,Vector2u(1900,1000));

    GameObjectGroup *tiles = generateMap(mapsize);
    engine->addGameObject(tiles);
    engine->set_setting_checkEventInterval(0.02);
    engine->set_setting_displayInterval(0.02);
    engine->set_setting_gameTickInterval(0.1);
    engine->setUserCheckEventLoop(userEventLoop);
    engine->setUserDisplayLoop(userDisplayLoop);

    displayText_pause = new DisplayText();
    displayText_pause->setCharacterSize(25);
    displayText_pause->setText("PAUSE (press space)");
    displayText_pause->setPos(Vector2f(engine->getWindowSize().x - displayText_pause->getText().getString().getSize()*(displayText_pause->getCharacterSize()-10),0));
    displayText_pause->setPositionFix(true);
    displayText_pause->setVisibility(true);
    engine->addDisplayText(displayText_pause);

    displayText_startInfo = new DisplayText();
    displayText_startInfo->setCharacterSize(25);
    displayText_startInfo->setText("Info");
    displayText_startInfo->setPos(Vector2f(0,0));
    displayText_startInfo->setPositionFix(true);
    displayText_startInfo->setVisibility(true);
    engine->addDisplayText(displayText_startInfo);

    engine->setup();

    keyEvent_P.setKey(KEYBOARD_KEY_P);
    keyEvent_SPCAE.setKey(KEYBOARD_KEY_SPACE);
    keyEvent_C.setKey(KEYBOARD_KEY_C);
    keyEvent_1.setKey(KEYBOARD_KEY_1);
    keyEvent_2.setKey(KEYBOARD_KEY_2);
    keyEvent_ENTER.setKey(KEYBOARD_KEY_ENTER);

  //  loadFromImage(resourcePath+"\\Simkin_glider_gun.png",Vector2u(40,20));
  //  loadFromImage(resourcePath+"\\Simkin_glider_gun.png",Vector2u(40,20));


    while(engine->running())
    {
        engine->checkEvent();
        if(!tickPause)
            engine->tick();
        engine->display();

#ifdef BUILD_WITH_EASY_PROFILER
        if(engine->getTick() > 20)
           engine->stop();
#endif
    }
#ifdef BUILD_WITH_EASY_PROFILER
    auto blocks_count = profiler::dumpBlocksToFile("profiler.prof");
    std::cout << "Profiler blocks count: " << blocks_count << std::endl;
#endif
}
void userEventLoop(float tickInterval,unsigned long long tick,const vector<sf::Event> &eventList)
{
    keyEvent_P.checkEvent();
    keyEvent_SPCAE.checkEvent();
    keyEvent_C.checkEvent();
    keyEvent_1.checkEvent();
    keyEvent_2.checkEvent();
    keyEvent_ENTER.checkEvent();

    if(keyEvent_P.isSinking())
    {
        engine->display_stats(keyEvent_P.getCounter_isSinking()%2);
    }
    if(keyEvent_SPCAE.isSinking())
    {
        tickPause = !tickPause;
        displayText_pause->setVisibility(tickPause);
    }
    if(keyEvent_C.isSinking())
    {
        clearMap();
    }
    if(keyEvent_1.isSinking())
    {
        loadFromImage(resourcePath+"\\Glider_gun.png",mapsize/2u-Vector2u(36,9)/2u);
    }
    if(keyEvent_2.isSinking())
    {
        loadFromImage(resourcePath+"\\Simkin_glider_gun.png",mapsize/2u-Vector2u(33,21)/2u);
    }
    if(keyEvent_ENTER.isSinking())
    {
        if(keyEvent_ENTER.getCounter_isSinking() == 1)
            engine->removeDisplayText(displayText_startInfo);
    }

    for(const sf::Event &event : eventList)
    {
        switch(event.type)
        {
            case sf::Event::MouseButtonPressed:
            {
                if(event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    leftMouseKilckActive = true;
                }else if(event.mouseButton.button == sf::Mouse::Button::Right)
                {
                    rightMouseKilckActive = true;
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                if(event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    leftMouseKilckActive = false;
                    setTileAlive(Vector2i(event.mouseButton.x,event.mouseButton.y));
                }
                if(event.mouseButton.button == sf::Mouse::Button::Right)
                {
                    rightMouseKilckActive = false;
                    setTileDead(Vector2i(event.mouseButton.x,event.mouseButton.y));
                }
                break;
            }
            case sf::Event::MouseMoved:
            {
                //qDebug() << "mouse: "<<event.mouseMove.x<<" "<<event.mouseMove.y;
                if(leftMouseKilckActive)
                {
                    setTileAlive(Vector2i(event.mouseMove.x,event.mouseMove.y));
                }
                if(rightMouseKilckActive)
                {
                    setTileDead(Vector2i(event.mouseMove.x,event.mouseMove.y));
                }
                break;
            }
            default:
            {}
        }
    }



}
void userDisplayLoop(float tickInterval,unsigned long long tick,PixelDisplay &display)
{
    display.addVertexLine(mapFrame.getDrawable(Color(0,0,255)));
}

void clearMap()
{
    for(unsigned int x=0; x<map.size(); x++)
    {
        for(unsigned int y=0; y<map[0].size(); y++)
        {
            map[x][y]->setDead();
        }
    }
}
void setTileAlive(Vector2i mousePos)
{
    bool outOfArea;
    Vector2u tile = getTilePos(mousePos,outOfArea);
    if(outOfArea)
        return;
    map[tile.x][tile.y]->setAlive();
}
void setTileDead(Vector2i mousePos)
{
    {
        bool outOfArea;
        Vector2u tile = getTilePos(mousePos,outOfArea);
        if(outOfArea)
            return;
        map[tile.x][tile.y]->setDead();
    }
}
Vector2u getTilePos(Vector2i mousePos,bool &outOfArea)
{
    Vector2u windowSize = engine->getWindowSize();
    RectF frame = engine->getRenderFrame();
    Vector2f relativePos((float)mousePos.x/(float)windowSize.x,(float)mousePos.y/(float)windowSize.y);
   // qDebug() << "relative: "<<relativePos.x<<" "<<relativePos.y;
    Vector2i tilePos(relativePos.x*frame.getSize().x+frame.getPos().x, relativePos.y*frame.getSize().y+frame.getPos().y);
   // qDebug() << "mouse: "<<mousePos.x<<" "<<mousePos.y;
    outOfArea = false;
    if(tilePos.x < 0 || (size_t)tilePos.x >= map.size())
        outOfArea = true;
    if(tilePos.y < 0 || (size_t)tilePos.y >= map[0].size())
        outOfArea = true;
    return Vector2u(tilePos);
}
GameObjectGroup *generateMap(Vector2u size)
{
    qDebug() << "generateMap begin";
    GameObjectGroup *group = new GameObjectGroup();


    for(unsigned int x=0; x<size.x; x++)
    {
        map.push_back(vector<Tile*>());
        for(unsigned int y=0; y<size.y; y++)
        {
            Tile *tile = new Tile(Vector2u(x,y));
            tile->setDead();
            tile->setColorFadeFactor(colorFadeFactor);
            map[x].push_back(tile);  
        }
    }
    mapFrame = RectF(0,0,size.x,size.y);

    for(unsigned int x=1; x<map.size()-1; x++)
    {
        for(unsigned int y=1; y<map[x].size()-1; y++)
        {
            map[x][y]->addNeighbour(map[x][y-1]);
            map[x][y]->addNeighbour(map[x-1][y-1]);
            map[x][y]->addNeighbour(map[x-1][y]);
            map[x][y]->addNeighbour(map[x-1][y+1]);
            map[x][y]->addNeighbour(map[x][y+1]);
            map[x][y]->addNeighbour(map[x+1][y+1]);
            map[x][y]->addNeighbour(map[x+1][y]);
            map[x][y]->addNeighbour(map[x+1][y-1]);

        }
    }
    size_t lastIndexY = map[0].size()-1;
    size_t lastIndexX = map.size()-1;
    for(unsigned int x=1; x<map.size()-1; x++)
    {
        map[x][0]->addNeighbour(map[x][1]);
        map[x][0]->addNeighbour(map[x-1][1]);
        map[x][0]->addNeighbour(map[x+1][1]);
        map[x][0]->addNeighbour(map[x+1][0]);
        map[x][0]->addNeighbour(map[x-1][0]);


        map[x][lastIndexY]->addNeighbour(map[x][lastIndexY-1]);
        map[x][lastIndexY]->addNeighbour(map[x-1][lastIndexY-1]);
        map[x][lastIndexY]->addNeighbour(map[x+1][lastIndexY-1]);
        map[x][lastIndexY]->addNeighbour(map[x+1][lastIndexY]);
        map[x][lastIndexY]->addNeighbour(map[x-1][lastIndexY]);
    }
    for(unsigned int y=1; y<map[0].size()-1; y++)
    {
        map[0][y]->addNeighbour(map[1][y]);
        map[0][y]->addNeighbour(map[1][y-1]);
        map[0][y]->addNeighbour(map[1][y+1]);
        map[0][y]->addNeighbour(map[0][y+1]);
        map[0][y]->addNeighbour(map[0][y-1]);


        map[lastIndexX][y]->addNeighbour(map[lastIndexX-1][y]);
        map[lastIndexX][y]->addNeighbour(map[lastIndexX-1][y-1]);
        map[lastIndexX][y]->addNeighbour(map[lastIndexX-1][y+1]);
        map[lastIndexX][y]->addNeighbour(map[lastIndexX  ][y+1]);
        map[lastIndexX][y]->addNeighbour(map[lastIndexX  ][y-1]);
    }
    map[0][0]->addNeighbour( map[1][0]);
    map[0][0]->addNeighbour( map[1][1]);
    map[0][0]->addNeighbour( map[0][1]);

    map[lastIndexX][0]->addNeighbour( map[lastIndexX-1][0]);
    map[lastIndexX][0]->addNeighbour( map[lastIndexX-1][1]);
    map[lastIndexX][0]->addNeighbour( map[lastIndexX][1]);

    map[lastIndexX][lastIndexY]->addNeighbour( map[lastIndexX-1][lastIndexY]);
    map[lastIndexX][lastIndexY]->addNeighbour( map[lastIndexX-1][lastIndexY-1]);
    map[lastIndexX][lastIndexY]->addNeighbour( map[lastIndexX][lastIndexY-1]);

    map[0][lastIndexY]->addNeighbour( map[1][lastIndexY]);
    map[0][lastIndexY]->addNeighbour( map[1][lastIndexY-1]);
    map[0][lastIndexY]->addNeighbour( map[0][lastIndexY-1]);

    for(unsigned int x=0; x<size.x; x++)
    {
        for(unsigned int y=0; y<size.y; y++)
        {

            group->add(map[x][y]);
        }
    }
    qDebug() << "generateMap end";
    return group;
}

void loadFromImage(const string &image,Vector2u drawPos)
{
    sf::Image sfImage;
    sfImage.loadFromFile(image);
    Vector2u imageSize = sfImage.getSize();

    if(drawPos.x + imageSize.x > map.size()-1)
        drawPos.x = map.size()-imageSize.x-1;
    if(drawPos.y + imageSize.y > map[0].size()-1)
        drawPos.y = map.size()-imageSize.y-1;

    for(size_t x=0; x<imageSize.x; x++)
    {
        for(size_t y=0; y<imageSize.y; y++)
        {
            Color col = sfImage.getPixel(x,y);
            if(col.a < 100 || (col.r == 0 && col.g == 0 && col.b == 0))
            {   // Don't add Pixels below this alpha value
               map[x+drawPos.x][y+drawPos.y]->setDead();
            }
            else
            {
                map[x+drawPos.x][y+drawPos.y]->setAlive(col);
            }
        }
    }
}
