#include "pixelengine.h"
#include "tile.h"
#include <math.h>

#include "QDebug"

const string resourcePath = "..\\..\\resources"; // Where your images are stored
const string outputPath   = "..\\..\\resources\\output"; // Where your images will be exported to
const Vector2u mapsize(500,500);    // Mapsize for the game
float colorFadeFactor = 0.7; // Factor how fast a dead cell will go to the color black. 1-> never, 0->instent
bool syncMode = true;
//string startInfoText = "Startinfo:\nKeybinding:\nKey   |   Description\nSPACE |   Will pause the game\nP     |   Will show some stats about the running engine\n1     |   Will load the "+resourcePath+"\\Glider_gun.png \n2     |   Will load the "+resourcePath+"\\Simkin_glider_gun.png\n3     |   Will load the "+resourcePath+"\\Netmaker.png\nC     |   Clears the map\nS     |   Saves the current map to a image in the Folder: "+outputPath+"\nA     |   Will load the last saved map\nENTER |   Will remove this infotext\n";
const string startInfoText =
        string("Startinfo:\n") +
        string("Keybinding:\n") +
        string("Key   |   Description\n") +
        string("SPACE |   Will pause the game\n") +
        string("P     |   Will show some stats about the running engine\n") +
        string("1     |   Will load the "+resourcePath+"\\Glider_gun.png\n") +
        string("2     |   Will load the "+resourcePath+"\\Simkin_glider_gun.png\n") +
        string("3     |   Will load the "+resourcePath+"\\Netmaker.png\n") +
        string("4     |   Will load the "+resourcePath+"\\frothing_spaceship.png\n") +
        string("5     |   Will load the "+resourcePath+"\\Sun.png\n") +
        string("C     |   Clears the map\n") +
        string("S     |   Saves the current map to an image in the Folder: "+outputPath+"\n") +
        string("A     |   Will load the last saved map\n") +
        string("-->   |   Will step one step forward\n") +
        string("ENTER |   Will remove this infotext\n") +
        string("\nMouse:\n")+
        string("WheelScroll:            Zoom +/-\n")+
        string("WheelKlick MouseMove:   Moves the Camera\n")+
        string("LeftKlick:              Sets the cell on the mouse cursor to alive\n")+
        string("RightKlick:             Sets the cell on the mouse cursor to dead\n");

PixelEngine *engine;
Event keyEvent_P;
Event keyEvent_SPCAE;
Event keyEvent_C;
Event keyEvent_S;
Event keyEvent_A;
Event keyEvent_ENTER;
Event keyEvent_singleStep;
vector<Event> keyEvent_numbers;
vector<vector<Tile*>    > map;
RectF mapFrame;
DisplayText *displayText_pause;
DisplayText *displayText_startInfo;


bool tickPause = true;
bool doSingleStep = false;
bool leftMouseKilckActive = false;
bool rightMouseKilckActive = false;
string lastSavedImagePath = "";

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
void saveToImage();
void saveToImage(const string &file);


int main(int argc, char *argv[])
{


    engine = new PixelEngine(mapsize,Vector2u(1900,1000));

    GameObjectGroup *tiles = generateMap(mapsize);
    engine->addGameObject(tiles);


    if(syncMode)
    {
        engine->set_setting_syncEngineInterval(0.02);
        engine->set_setting_runInSync(true);
    }
    else
    {
        engine->set_setting_checkEventInterval(0.02);
        engine->set_setting_displayInterval(0.01);
        engine->set_setting_gameTickInterval(0.01);
    }
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
    displayText_startInfo->setCharacterSize(18);
    displayText_startInfo->setText(startInfoText);
    displayText_startInfo->setPos(Vector2f(0,0));
    displayText_startInfo->setPositionFix(true);
    displayText_startInfo->setVisibility(true);
    engine->addDisplayText(displayText_startInfo);

    engine->setup();

    keyEvent_P.setKey(KEYBOARD_KEY_P);
    keyEvent_SPCAE.setKey(KEYBOARD_KEY_SPACE);
    keyEvent_C.setKey(KEYBOARD_KEY_C);
    keyEvent_S.setKey(KEYBOARD_KEY_S);
    keyEvent_A.setKey(KEYBOARD_KEY_A);
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_1));
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_2));
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_3));
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_4));
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_5));
    keyEvent_ENTER.setKey(KEYBOARD_KEY_ENTER);
    keyEvent_singleStep.setKey(KEYBOARD_KEY_ARROW_RIGHT);


#ifdef BUILD_WITH_EASY_PROFILER
    tickPause = false;
    loadFromImage(resourcePath+"\\Netmaker.png",mapsize/2u-Vector2u(49,26)/2u);
#endif

    while(engine->running())
    {
        engine->checkEvent();
        if(!tickPause || doSingleStep)
        {
            doSingleStep = false;
            engine->tick();
        }
        engine->display();
#ifdef BUILD_WITH_EASY_PROFILER
        if(engine->getTick() > 20)
           engine->stop();
#endif
    }
/*#ifdef BUILD_WITH_EASY_PROFILER
    auto blocks_count = profiler::dumpBlocksToFile("profiler.prof");
    std::cout << "Profiler blocks count: " << blocks_count << std::endl;
#endif*/
    delete engine;
}
void userEventLoop(float tickInterval,unsigned long long tick,const vector<sf::Event> &eventList)
{
    keyEvent_P.checkEvent();
    keyEvent_SPCAE.checkEvent();
    keyEvent_C.checkEvent();
    keyEvent_S.checkEvent();
    keyEvent_A.checkEvent();
    for(Event & e:keyEvent_numbers)
        e.checkEvent();
    keyEvent_ENTER.checkEvent();
    keyEvent_singleStep.checkEvent();

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
    if(keyEvent_S.isSinking())
    {
        saveToImage();
    }
    if(keyEvent_A.isSinking())
    {
        if(lastSavedImagePath == "")
        {
            qDebug() <<"No file exported";
        }
        else
        {
            qDebug() << "load File: "<<lastSavedImagePath.c_str();
            loadFromImage(lastSavedImagePath,Vector2u(0,0));
        }
    }
    for(size_t i=0;i<keyEvent_numbers.size(); i++)
    {
        if(keyEvent_numbers[i].isSinking())
        {
            switch(i)
            {
                case 0: // Taste "1"
                    loadFromImage(resourcePath+"\\Glider_gun.png",mapsize/2u-Vector2u(36,9)/2u);
                break;
                case 1: // Taste "1"
                    loadFromImage(resourcePath+"\\Simkin_glider_gun.png",mapsize/2u-Vector2u(33,21)/2u);
                break;
                case 2: // Taste "1"
                    loadFromImage(resourcePath+"\\Netmaker.png",mapsize/2u-Vector2u(49,26)/2u);
                break;
                case 3:
                    loadFromImage(resourcePath+"\\frothing_spaceship.png",mapsize/2u-Vector2u(33,23)/2u);
                break;
                case 4:
                    loadFromImage(resourcePath+"\\Sun.png",mapsize/2u-Vector2u(33,23)/2u);
                break;
                default:

                break;
            }
        }
    }
    if(keyEvent_ENTER.isSinking())
    {
        if(keyEvent_ENTER.getCounter_isSinking() == 1)
            engine->removeDisplayText(displayText_startInfo);
    }
    if(keyEvent_singleStep.isSinking())
    {
        doSingleStep = true;
    }else

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

    if(drawPos.x + imageSize.x > map.size())
        drawPos.x = map.size()-imageSize.x;
    if(drawPos.y + imageSize.y > map[0].size())
        drawPos.y = map.size()-imageSize.y;

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
void saveToImage()
{
    string path     = outputPath;
    std::time_t t   = std::time(0);   // get time now
    std::tm* now    = std::localtime(&t);
    string timeDate = to_string(now->tm_mday)+"."+to_string(now->tm_mon)+"."+to_string(now->tm_year)+"_";
    timeDate       += to_string(now->tm_hour)+"."+to_string(now->tm_min)+"."+to_string(now->tm_sec)+"_";
    saveToImage(path + "\\" + timeDate +to_string(engine->getTick()) + ".png" );
}
void saveToImage(const string &file)
{
    sf::Image image;
    image.create(map.size(),map[0].size(),Color(0,0,0,0));

    for(size_t x=0; x<map.size(); x++)
    {
        for(size_t y=0; y<map[x].size(); y++)
        {
            if(map[x][y]->getState() == Tile::dead)
                continue;
            Color col = map[x][y]->getColor();
            if(col.r > 0 || col.g > 0 || col.b > 0)
                image.setPixel(x,y,col);
        }
    }
    lastSavedImagePath = file;
    qDebug() << "Saving to: "<<file.c_str();
    image.saveToFile(file);
}
