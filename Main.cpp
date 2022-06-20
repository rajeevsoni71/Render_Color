#include "Render_Color.h"
#include<unistd.h>
int main (){
    Render_Color renderObj;
    if(renderObj.activate()){
        if(renderObj.drawFrame()){
            renderObj.deactivate();
        }
    }
   
    return 0;
}