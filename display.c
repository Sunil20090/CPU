#include <stdio.h>
#include <stdlib.h>

typedef struct Screen{
    struct Screen *pParent;
    int childCount;
    struct Screen *pChilderen[10];
    char* name;
} Screen;


void addChild(Screen *pParent, Screen *child){
    child->pParent = pParent;
    pParent->pChilderen[pParent->childCount++] = child;
}

Screen* createScreen(char* name){
    Screen *screen = malloc(sizeof(Screen));
    screen->name = name;
    screen->childCount = 0;
    return screen;
}


void printScreen(Screen *screen, int depth){
    if(depth > 10){
        return;
    }
    printf("%s[%d]\n", screen->name, screen->childCount);
    // printf("COUNT: %d", screen->childCount);
    int i=0;

    while ( i < screen->childCount )
    {
        int j = 0 ;
        while (j < depth)
        {
            printf(" |-----");
            
            j++;
        }

        printScreen(screen->pChilderen[i], depth + 1);

        i++;
    }
}

int main(){
    Screen *add = createScreen("Add");
    Screen *home = createScreen("HOME");
    // addChild(add, home);
    // Screen *gpio = createScreen("GPIO");
    // addChild(home, gpio);
    // Screen *i2c = createScreen("i2c");
    // addChild(home, i2c);

    // addChild(i2c, createScreen("adress"));
    // addChild(i2c, createScreen("data"));

    // addChild(home, createScreen("spi"));
    // addChild(home, createScreen("uart"));
    // Screen *port = createScreen("Port");
    // Screen *pin = createScreen("Pin");
    // Screen *state = createScreen("State");
    // addChild(gpio, port);
    // addChild(gpio, pin);
    // addChild(gpio, state);

    // addChild(port, createScreen("GPIOA"));
    // addChild(port, createScreen("GPIOB"));
    // addChild(port, createScreen("GPIOC"));
    // addChild(port, createScreen("GPIOD"));


    // addChild(pin, createScreen("PIN_1"));
    // addChild(pin, createScreen("PIN_2"));
    // addChild(pin, createScreen("PIN_3"));
    // addChild(pin, createScreen("PIN_4"));
    // addChild(pin, createScreen("PIN_5"));
    // addChild(pin, createScreen("PIN_6"));
    // addChild(pin, createScreen("PIN_7"));

    // addChild(state, createScreen("HIGH"));
    // addChild(state, createScreen("LOW"));


    char* name = "SONU KUMAR";

    int i=-1; 
    while(name[++i]);

    printf("Length is %d\n", i);

    void (*funcPtr)(Screen*, Screen*);

    funcPtr = &addChild;

    funcPtr(add, home);

    printScreen(add, 1);

    return 0;

}