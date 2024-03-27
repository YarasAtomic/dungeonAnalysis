#include "rayui.h"
#include <iostream>

void UI::add(UIComponent*component){
    components.push_back(component);
}

void UI::update(const UIState * uiState,int width,int height){
    for(int i = 0 ; i < components.size(); i++){
        components[i]->update(uiState,width,height);
    }
}

void UI::update(int width,int height){
    UIState * uiState = new UIState {};//! Leak
    uiState->clicked = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    uiState->mouse = GetMousePosition();
    for(int i = 0 ; i < components.size(); i++){
        components[i]->update(uiState,width,height);
    }
}

void UI::draw(int screenWidth,int screenHeight){
    for(int i = 0 ; i < components.size(); i++){
        components[i]->draw();
    }
}

UIComponent::UIComponent(Vector2 relPos,Vector2 size,Vector2 screenSize) {
    children = new UI;
    this->percentagePos = {relPos.x/screenSize.x,relPos.y/screenSize.y};
    this->relPos = relPos;
    rectangle = {relPos.x,relPos.y,size.x,size.y};
}

void UIComponent::addChild(UIComponent * child){
    children->add(child);
    child->parent = this;
}

Vector2 UIComponent::calculateAbsolutePos(int screenWidth,int screenHeight){
    Vector2 pos = {0,0};
    if(parent != nullptr){
        pos.x = parent->rectangle.x;
        pos.y = parent->rectangle.y;
    }
    switch(anchor){
    case UP:
        std::cout << "percetage " << percentagePos.x << " width " << screenWidth << std::endl;
        pos.x += screenWidth*percentagePos.x;
        pos.y += relPos.y;
        break;
    case DOWN:
        pos.x += screenWidth*percentagePos.x;
        pos.y += screenHeight - relPos.y;
        break;
    case LEFT:
        pos.x += relPos.x;
        pos.y += screenHeight*percentagePos.y;
        break;
    case RIGHT:
        pos.x += screenWidth - relPos.x;
        pos.y += relPos.y*percentagePos.y;
        break;
    case UPLEFT:
        pos.x += relPos.x;
        pos.y += relPos.y;
        break;
    case DOWNLEFT:
        pos.x += relPos.x;
        pos.y += screenHeight - relPos.y;
        break;
    case UPRIGHT:
        pos.x += screenWidth - relPos.x;
        pos.y += relPos.y;
        break;
    case DOWNRIGHT:
        pos.x += screenWidth - relPos.x;
        pos.y += screenHeight - relPos.y;
        break;
    }
    return pos;
}

Button::Button(Vector2 relPos,Vector2 size,Color color,Color hoverColor,Color clickedColor,std::function<void()> callback,Vector2 screenSize) 
: UIComponent{relPos,size,screenSize}{ 
    this->color = color;
    this->hoverColor = hoverColor;
    this->clickedColor = clickedColor;
    this->callback=callback;
    this->buttonType = textButton;
}

void Button::update(const UIState * uiState,int width,int height){
    if(delayTimer>0) delayTimer--;
    state = Button::ButtonState::none;

    Vector2 pos = calculateAbsolutePos(width,height);
    rectangle.x = pos.x;
    rectangle.y = pos.y;

    if(CheckCollisionPointRec(uiState->mouse, rectangle)) {
        if(!uiState->clicked){
            state = Button::ButtonState::hover;
        }else{
            state = Button::ButtonState::clicked;

            if(delayTimer==0){
                delayTimer = DELAY;
                callback();
                return ;
            }
            delayTimer = DELAY;
        }
    }

    children->update(uiState,width,height);
}

void Button::draw(){
    Color drawColor = color;
    if(delayTimer>0) drawColor = clickedColor;
    if(state == Button::ButtonState::hover) {
        drawColor = hoverColor;
    }
    DrawRectangle(rectangle.x,rectangle.y,rectangle.width,rectangle.height,drawColor);
    int centerX = rectangle.x + rectangle.width/2;
    int centerY = rectangle.y + rectangle.height/2;
    Vector2 size = MeasureTextEx(font,data.c_str(),fontSize,spacing);
    DrawTextEx(font,data.c_str(),{(float)(centerX-size.x/2),(float)(centerY-size.y/2)},fontSize,spacing,RED);

    children->draw(size.x,size.y);
}

UIText::UIText(Vector2 relPos,Vector2 size,std::string text,int fontSize,int spacing,Color color ,Font font,Vector2 screenSize)
: UIComponent{relPos,size,screenSize}{
    this->font = font;
    this->color = color;
    this->spacing = spacing;
    this->fontSize = fontSize;
    this->text = text;
}

void UIText::update(const UIState * uiState,int width,int height){
    Vector2 pos = calculateAbsolutePos(width,height);
    rectangle.x = pos.x;
    rectangle.y = pos.y;

    children->update(uiState,width,height);
}

void UIText::draw(){
    int centerX = rectangle.x + rectangle.width/2;
    int centerY = rectangle.y + rectangle.height/2;
    Vector2 size = MeasureTextEx(font,text.c_str(),fontSize,spacing);
    DrawTextEx(font,text.c_str(),{(float)(centerX-size.x/2),(float)(centerY-size.y/2)},fontSize,spacing,RED);

    children->draw(size.x,size.y);
}