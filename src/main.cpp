#define ENGINE_DISABLE_COLLISION
#include "pixelengine.h"
#include "tile.h"
#include <math.h>

#include "QDebug"
//#define RELEASE_COMPILE

#ifdef RELEASE_COMPILE
string resourcePath = "resources"; // Where your images are stored
string outputPath   = "resources\\output"; // Where your images will be exported to
#else
string resourcePath = "..\\..\\resources"; // Where your images are stored
string outputPath   = "..\\..\\resources\\output"; // Where your images will be exported to
#endif
string progExecPath;
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
        string("O     |   Will open the filebrowser, so you can load images\n") +
        string("      |   Move the mouse for positioning the image, rotate by right click and placing by left click.\n") +
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
Event keyEvent_ESCAPE;

// Insert a specific image
Event keyEvent_O;

bool insertingImageMode = false;
int insertingImageRotation = 0;
std::string insertingImagePath = "";
GameObject *insertingImageObj;
Vector2u insertingImageSize;
//--------


vector<vector<Tile*>    > map;
RectF mapFrame;
TextPainter *displayText_pause;
TextPainter *displayText_startInfo;
VertexPathPainter *mapBorder;



bool tickPause = true;
bool doSingleStep = false;
bool leftMouseKilckActive = false;
bool rightMouseKilckActive = false;
string lastSavedImagePath = "";

GameObjectGroup *generateMap(Vector2u size);

void clearMap();
void setTileAlive(Vector2i mousePos);
void setTileDead(Vector2i mousePos);
Vector2i getTilePos(Vector2i mousePos);


// These functions will later be called from the engine
// userEventLoop: Here you can handle your Events (KeyEvents).
void userEventLoop(float tickInterval,unsigned long long tick,const vector<sf::Event> &eventList);
void userDisplayLoop(float tickInterval,unsigned long long tick,PixelDisplay &display);

void loadFromImage(const string &image,Vector2i drawPos,Origin origin,int rotation = 0);
void saveToImage();
void saveToImage(const string &file);


int main(int argc, char *argv[])
{

    progExecPath = argv[0];
#ifdef RELEASE_COMPILE
    std::string tmpPath = progExecPath;
    tmpPath = tmpPath.substr(0,tmpPath.rfind("\\"));
    resourcePath = tmpPath + "\\" + resourcePath;
    outputPath   = tmpPath + "\\" + outputPath;
#else
    std::string tmpPath = progExecPath;
    tmpPath = tmpPath.substr(0,tmpPath.rfind("\\"));
    tmpPath = tmpPath.substr(0,tmpPath.rfind("\\"));
    tmpPath = tmpPath.substr(0,tmpPath.rfind("\\"));
    tmpPath = tmpPath.substr(0,tmpPath.rfind("\\"));
    resourcePath = tmpPath+"\\resources"; // Where your images are stored
    outputPath   = tmpPath+"\\resources\\output"; // Where your images will be exported to
#endif


    PixelEngine::Settings settings  = PixelEngine::getSettings();
#ifdef RELEASE_COMPILE
    float windowScale = 1;
#else
    //float windowScale = 1;
    float windowScale = 2;
#endif
    settings.display.windowSize     = Vector2u(1900*windowScale,1000*windowScale);
    settings.display.pixelMapSize   = mapsize;

    engine = new PixelEngine(settings);
    engine->setIcon(resourcePath+"\\icon.png");
    engine->setTitle("Conway's game of life");


    mapBorder = new VertexPathPainter();

    GameObjectGroup *tiles = generateMap(mapsize);
    engine->addGameObject(tiles);


    if(syncMode)
    {
        engine->set_setting_syncEngineInterval(0);
        engine->set_setting_runInSync(true);
    }
    else
    {
        engine->set_setting_checkEventInterval(1.f/20.f);
        engine->set_setting_displayInterval(1.f/20.f);
        engine->set_setting_gameTickInterval(0.0);
        engine->set_setting_runInSync(false);
    }
    engine->setUserCheckEventLoop(userEventLoop);
    engine->setUserDisplayLoop(userDisplayLoop);

    displayText_pause = new TextPainter();
    displayText_pause->setCharacterSize(25);
    displayText_pause->setText("PAUSE (press space)");
    displayText_pause->setPos(Vector2f(engine->getWindowSize().x - displayText_pause->getText().getString().getSize()*(displayText_pause->getCharacterSize()-10),0));
    displayText_pause->setPositionFix(true);
    displayText_pause->setVisibility(true);
    engine->addPainterToDisplay(displayText_pause);

    displayText_startInfo = new TextPainter();
    displayText_startInfo->setCharacterSize(18);
    displayText_startInfo->setText(startInfoText);
    displayText_startInfo->setPos(Vector2f(0,0));
    displayText_startInfo->setPositionFix(true);
    displayText_startInfo->setVisibility(true);
    displayText_startInfo->setScale(1);
    engine->addPainterToDisplay(displayText_startInfo);

    engine->addPainterToDisplay(mapBorder);

    // Set keyevents
    keyEvent_P.setKey(KEYBOARD_KEY_P);
    keyEvent_SPCAE.setKey(KEYBOARD_KEY_SPACE);
    keyEvent_C.setKey(KEYBOARD_KEY_C);
    keyEvent_S.setKey(KEYBOARD_KEY_S);
    keyEvent_A.setKey(KEYBOARD_KEY_A);
    keyEvent_O.setKey(KEYBOARD_KEY_O);
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_1));
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_2));
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_3));
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_4));
    keyEvent_numbers.push_back(Event(KEYBOARD_KEY_5));
    keyEvent_ENTER.setKey(KEYBOARD_KEY_ENTER);
    keyEvent_singleStep.setKey(KEYBOARD_KEY_ARROW_RIGHT);
    keyEvent_ESCAPE.setKey(KEYBOARD_KEY_ESCAPE);

    engine->addEvent(&keyEvent_P);
    engine->addEvent(&keyEvent_SPCAE);
    engine->addEvent(&keyEvent_C);
    engine->addEvent(&keyEvent_S);
    engine->addEvent(&keyEvent_A);
    engine->addEvent(&keyEvent_O);
    engine->addEvent(&keyEvent_ENTER);
    engine->addEvent(&keyEvent_singleStep);
    engine->addEvent(&keyEvent_ESCAPE);
    for(Event & e:keyEvent_numbers)
        engine->addEvent(&e);

    engine->setup();





#ifdef BUILD_WITH_EASY_PROFILER
    tickPause = false;
    loadFromImage(resourcePath+"\\Netmaker.png",mapsize/2u-Vector2u(49,26)/2u);
    for(size_t x=0; x<map.size(); x++)
    {
        for(size_t y=0; y<map[x].size(); y++)
        {
            map[x][y]->setPause(tickPause);
        }
    }
#endif

    while(engine->running())
    {
        engine->loop();
#ifdef BUILD_WITH_EASY_PROFILER
        if(engine->getTick() > 20)
           engine->stop();
#endif
    }
    delete engine;
}
void userEventLoop(float tickInterval,unsigned long long tick,const vector<sf::Event> &eventList)
{
    if(keyEvent_P.isSinking())
    {
        engine->display_stats(keyEvent_P.getCounter_isSinking()%2);
    }
    if(keyEvent_SPCAE.isSinking())
    {
        tickPause = !tickPause;
        displayText_pause->setVisibility(tickPause);
        for(size_t x=0; x<map.size(); x++)
        {
            for(size_t y=0; y<map[x].size(); y++)
            {
                map[x][y]->setPause(tickPause);
            }
        }
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
            loadFromImage(lastSavedImagePath,Vector2i(0,0),Origin::topLeft);
        }
    }
    for(size_t i=0;i<keyEvent_numbers.size(); i++)
    {
        if(keyEvent_numbers[i].isSinking())
        {
            switch(i)
            {
                case 0: // Taste "1"
                    loadFromImage(resourcePath+"\\Glider_gun.png",Vector2i(mapsize/2u)-Vector2i(36,9)/2,Origin::topLeft);
                break;
                case 1: // Taste "1"
                    loadFromImage(resourcePath+"\\Simkin_glider_gun.png",Vector2i(mapsize/2u)-Vector2i(33,21)/2,Origin::topLeft);
                break;
                case 2: // Taste "1"
                    loadFromImage(resourcePath+"\\Netmaker.png",Vector2i(mapsize/2u)-Vector2i(49,26)/2,Origin::topLeft);
                break;
                case 3:
                    loadFromImage(resourcePath+"\\frothing_spaceship.png",Vector2i(mapsize/2u)-Vector2i(33,23)/2,Origin::topLeft);
                break;
                case 4:
                    loadFromImage(resourcePath+"\\Sun.png",Vector2i(mapsize/2u)-Vector2i(33,23)/2,Origin::topLeft);
                break;
                default:

                break;
            }
        }
    }
    if(keyEvent_O.isSinking())
    {
        if(!insertingImageMode)
        {
            insertingImagePath = FileBrowser::getFile(resourcePath);
            if(insertingImagePath == "")// No file selected
                goto noImageInsertion;

            sf::Image testImage;
            if(!testImage.loadFromFile(insertingImagePath))
            {
                // File not existent
                goto noImageInsertion;
            }
            insertingImageMode = true;

            Texture *texture = new Texture();
            TexturePainter *texPainter = new TexturePainter();
            texture->loadTexture(insertingImagePath);
            insertingImageSize = texture->getSize();

            texture->setOriginType(Origin::center);
            texPainter->setTexture(texture);

            insertingImageObj = new GameObject();
            insertingImageObj->addPainter(texPainter);
            insertingImageObj->setRenderLayer(RenderLayerIndex::vertexPaths);
            insertingImageObj->setPosInitial(Vector2f(-5,-5));
            engine->addGameObject(insertingImageObj);

            //loadFromImage(filePath,Vector2u(0,0));
        }
        noImageInsertion:;
    }
    if(keyEvent_ESCAPE.isSinking())
    {
        if(insertingImageMode)
        {
            engine->removeGameObject(insertingImageObj);
            insertingImageMode     = false;
            insertingImageRotation = 0;
        }
    }
    if(keyEvent_ENTER.isSinking())
    {
        if(keyEvent_ENTER.getCounter_isSinking() == 1)
            engine->removePainterFromDisplay(displayText_startInfo);
    }
    if(keyEvent_singleStep.isSinking())
    {
        doSingleStep = true;
        if(tickPause)
        {
            for(size_t x=0; x<map.size(); x++)
            {
                for(size_t y=0; y<map[x].size(); y++)
                {
                    map[x][y]->setPause(false);
                }
            }
            engine->loop();
            for(size_t x=0; x<map.size(); x++)
            {
                for(size_t y=0; y<map[x].size(); y++)
                {
                    map[x][y]->setPause(true);
                }
            }
        }
    }

    for(const sf::Event &event : eventList)
    {
        switch(event.type)
        {
            case sf::Event::MouseButtonPressed:
            {
                if(insertingImageMode)
                {
                    if(event.mouseButton.button == sf::Mouse::Button::Right)
                    {
                        insertingImageRotation ++;
                        insertingImageRotation = insertingImageRotation%4;
                        insertingImageObj->rotate_90();
                    }
                    if(event.mouseButton.button == sf::Mouse::Button::Left)
                    {
                        //insertingImageObj->setPos(Vector2f(getTilePos(Vector2i(event.mouseButton.x,event.mouseButton.y))));
                        loadFromImage(insertingImagePath,
                                      getTilePos(Vector2i(event.mouseButton.x,event.mouseButton.y)),
                                      Origin::center,
                                      insertingImageRotation);
                    }
                }
                else
                {
                    if(event.mouseButton.button == sf::Mouse::Button::Left)
                    {
                        leftMouseKilckActive = true;
                    }else if(event.mouseButton.button == sf::Mouse::Button::Right)
                    {
                        rightMouseKilckActive = true;
                    }
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                if(insertingImageMode)
                {

                }
                else
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
                }
                break;
            }
            case sf::Event::MouseMoved:
            {
                if(insertingImageMode)
                {
                    insertingImageObj->setPos(Vector2f(getTilePos(Vector2i(event.mouseMove.x,event.mouseMove.y))));
                }
                else
                {
                    if(leftMouseKilckActive)
                    {
                        setTileAlive(Vector2i(event.mouseMove.x,event.mouseMove.y));
                    }
                    if(rightMouseKilckActive)
                    {
                        setTileDead(Vector2i(event.mouseMove.x,event.mouseMove.y));
                    }
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
    //display.addVertexLine(mapFrame.getDrawable(Color(0,0,255)));
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
    Vector2i tile = getTilePos(mousePos);
    map[tile.x][tile.y]->setAlive();
}
void setTileDead(Vector2i mousePos)
{

    Vector2i tile = getTilePos(mousePos);
    map[tile.x][tile.y]->setDead();

}
Vector2i getTilePos(Vector2i mousePos)
{
    Vector2u windowSize = engine->getWindowSize();
    RectF frame = engine->getRenderFrame();
    Vector2f relativePos((float)mousePos.x/(float)windowSize.x,(float)mousePos.y/(float)windowSize.y);
   // qDebug() << "relative: "<<relativePos.x<<" "<<relativePos.y;
    Vector2i tilePos(relativePos.x*frame.getSize().x+frame.getPos().x, relativePos.y*frame.getSize().y+frame.getPos().y);
   // qDebug() << "mouse: "<<mousePos.x<<" "<<mousePos.y;


    if(tilePos.x < 0)
        tilePos.x = tilePos.x + map.size();
    if(tilePos.y < 0)
        tilePos.y = tilePos.y + map[0].size();

    tilePos.x = tilePos.x%map.size();
    tilePos.y = tilePos.y%map[0].size();

    return Vector2i(tilePos);
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
            //tile->setDead();
            tile->setColorFadeFactor(colorFadeFactor);
            map[x].push_back(tile);  
        }
    }
    qDebug() << "generateMap end";
    qDebug() << "connect the tiles begin";
    mapFrame = RectF(0,0,size.x,size.y);
    mapBorder->addPath(mapFrame.getDrawable());
    for(unsigned int x=0; x<map.size(); x++)
    {
        for(unsigned int y=0; y<map[x].size(); y++)
        {
            int xLeft, xRight, yAbove, yBelow;

            if(x == 0)
                xLeft = size.x -1;
            else
                xLeft = -1;

            if(x == (size.x - 1))
                xRight = -(size.x - 1);
            else
                xRight = 1;

            if(y == 0)
                yAbove = size.y - 1;
            else
                yAbove = -1;

            if(y == (size.y - 1))
                yBelow = -(size.y - 1);
            else
                yBelow = 1;

            map[x][y]->addneighbor(map[x+xLeft][y+yAbove]);
            map[x][y]->addneighbor(map[x][y+yAbove]);
            map[x][y]->addneighbor(map[x+xLeft][y]);
            map[x][y]->addneighbor(map[x+xRight][y]);
            map[x][y]->addneighbor(map[x+xLeft][y+yBelow]);
            map[x][y]->addneighbor(map[x+xRight][y+yAbove]);
            map[x][y]->addneighbor(map[x][y+yBelow]);
            map[x][y]->addneighbor(map[x+xRight][y+yBelow]);
        }
    }
    qDebug() << "connect the tiles end";
    qDebug() << "add to group begin";
    for(unsigned int x=0; x<size.x; x++)
    {
        for(unsigned int y=0; y<size.y; y++)
        {
            group->add(map[x][y]);
        }
    }
    qDebug() << "add to group end";
    return group;
}

void loadFromImage(const string &image,Vector2i drawPos,Origin origin,int rotation)
{
    sf::Image sfImage;
    sfImage.loadFromFile(image);
    Vector2u imageSize = sfImage.getSize();
    Vector2i orientationOriginOffset;



    for(size_t x=0; x<imageSize.x; x++)
    {
        for(size_t y=0; y<imageSize.y; y++)
        {
            Color col = sfImage.getPixel(x,y);
            int rotatedX = 0;
            int rotatedY = 0;


            switch(rotation)
            {
                case 0:
                    rotatedX = x;
                    rotatedY = y;

                    orientationOriginOffset.x = -imageSize.x;
                    orientationOriginOffset.y = -imageSize.y;
                break;
                case 1:
                    rotatedX = -y-1;
                    rotatedY = x;

                    orientationOriginOffset.x =  imageSize.y;
                    orientationOriginOffset.y = -imageSize.x;
                break;
                case 2:
                    rotatedX = -x-1;
                    rotatedY = -y-1;

                    orientationOriginOffset.x = imageSize.x;
                    orientationOriginOffset.y = imageSize.y;
                break;
                case 3:
                    rotatedX = y;
                    rotatedY = -x-1;

                    orientationOriginOffset.x = -imageSize.y;
                    orientationOriginOffset.y =  imageSize.x;
                break;
            }
            switch(origin)
            {
                case Origin::topLeft:
                break;
                case Origin::topRight:
                    rotatedX += orientationOriginOffset.x;
                break;
                case Origin::bottomLeft:
                    rotatedY += orientationOriginOffset.y;
                break;
                case Origin::bottomRight:
                    rotatedX += orientationOriginOffset.x;
                    rotatedY += orientationOriginOffset.y;
                break;
                case Origin::center:
                    rotatedX += orientationOriginOffset.x/2;
                    rotatedY += orientationOriginOffset.y/2;
                break;
                default:
                break;
            }

            if(col.a < 100 || (col.r == 0 && col.g == 0 && col.b == 0))
            {   // Don't add Pixels below this alpha value
            //     map[(x+drawPos.x)%map.size()][(y+drawPos.y)%map[0].size()]->setDead();
            }
            else
            {
                int posX = (drawPos.x + rotatedX + map.size()) % map.size();
                int posY = (drawPos.y + rotatedY + map[0].size()) % map[0].size();

               /* if(rotatedX < 0)
                    rotatedX = rotatedY + map.size();
                if(rotatedY < 0)
                    rotatedY = rotatedY + map[0].size();

                rotatedX = rotatedX%map.size();
                rotatedY = rotatedY%map[0].size();*/

                map[posX][posY]->setAlive(col);
            }
        }
    }
}
void saveToImage()
{
    string path     = outputPath;
    std::time_t t   = std::time(0);   // get time now
    std::tm* now    = std::localtime(&t);
    string timeDate = to_string(now->tm_mday)+"_"+to_string(now->tm_mon+1)+"_"+to_string(now->tm_year+1900)+"_";
    timeDate       += to_string(now->tm_hour)+"_"+to_string(now->tm_min)+"_"+to_string(now->tm_sec)+"_";
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
