#include<vector>
#include<functional>
#include<raylib.h>
#include<string>

enum Anchor {UP, DOWN, LEFT, RIGHT, UPLEFT, UPRIGHT, DOWNLEFT, DOWNRIGHT};

struct UIState{
    Vector2 mouse;
    bool clicked;
};

class UI;
class UIComponent{
protected:
    UIComponent * parent = nullptr;
    UI * children;

    Anchor anchor = UPLEFT;
    Vector2 relPos;
    Rectangle rectangle;
    Vector2 percentagePos;

    Vector2 calculateAbsolutePos(int,int);
public:
    UIComponent(Vector2 relPos,Vector2 size,Vector2 screenSize);
    virtual void draw() = 0;
    virtual void update(const UIState *,int,int) = 0;
    void addChild(UIComponent *);
};

class UI{
    std::vector<UIComponent*> components;
public:
    void add(UIComponent*);
    void update(int,int);
    void update(const UIState *,int,int);
    void draw(int,int);
};

enum ButtonType {textButton,imgButton};

class Button : public UIComponent{
private:
    //Atributes
    std::string data;
    Color color;
    Color hoverColor;
    Color clickedColor;
    std::function<void()> callback;
    ButtonType buttonType;
    Font font;
    int fontSize;
    int spacing;

    //States
    enum ButtonState {none,hover,clicked};
    ButtonState state = none;
    int delayTimer = 0;
    const int DELAY = 10;
    
public:
    Button(Vector2,Vector2,Color,Color,Color,std::function<void()>,Vector2 screenSize);
    void update(const UIState *,int,int);
    void draw();
};

class UIText : public UIComponent{
    std::string text;
    Color color;
    Font font;
    int fontSize;
    int spacing;

public:
    UIText(Vector2 relPos,Vector2 size,std::string,int fontSize,int spacing,Color,Font,Vector2 screenSize);
    void update(const UIState *,int,int);
    void draw();
};