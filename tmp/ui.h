#include <vector>
#include <memery>

class UI{
using IWidgetPtr = std::shared_ptr<IWidget>;
public:
    UI();
    ~UI();
    void AddWidget(IWidgetPtr widget);
    
private:
    std::vector<IWidgetPtr> widgets;
};

UI::UI(){
    GetInputer().AddListenner(this, [](){
        
    });
}

void UI::AddWidget(IWidget widget){
    this->widgets.push_back(widget);
}