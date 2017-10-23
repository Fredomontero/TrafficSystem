#include <tchar.h>
#include <windows.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#define MAX 54
#define DATA 8
using namespace std;

/*Variables declaration*/
HWND ventana, outputs[2][54], inputs[5][54], label1, label2, label3;
uint8_t inputData[5][54], frame[MAX], aux[MAX], mask, temp;
int i, j;

/*needed C Functions*/
extern "C"{
    void configSocket(void);
    void receptionUDP(uint8_t frame[]);
    void responUDP(uint8_t frame[]);
}

/*Functions prototypes*/
int validateFrame(void);
void updateLabel(uint8_t index);
void sendInputs(uint8_t index);
void updateLabel1(void);
void updateLabel2(void);

/*Implementation of threads*/
DWORD WINAPI ThreadFunc(void* data) {
    while(1){
        receptionUDP(frame);
        validateFrame();
    }
}

/*Events*/
LRESULT CALLBACK winProc(HWND hwnd, UINT msj, WPARAM wParam, LPARAM lParam){
    switch(msj){
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            for(j = 0; j<7; j++){
                for(i = 0; i<54; i++){
                    if((HWND)lParam == inputs[j][i]){
                        if(inputData[j][i] == 1){
                            SetWindowText(inputs[j][i],"0");
                            inputData[j][i] = 0;
                        }else{
                            SetWindowText(inputs[j][i],"1");
                            inputData[j][i] = 1;
                        }
                    }
                }
            }
            break;
    }
    DefWindowProc(hwnd, msj, wParam, lParam);
    return 1;
}

/*More variables*/
char app[] = "myClass";
long int var = clock();


/*Main function*/
int WINAPI WinMain(HINSTANCE ins, HINSTANCE ins2, LPSTR cmd, int estado){

    /*The main window and it's attributes*/
    WNDCLASSEX vtn;
    vtn.cbClsExtra = 0;
    vtn.cbSize = sizeof(WNDCLASSEX);
    vtn.cbWndExtra = 0;
    vtn.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
    vtn.hCursor = LoadCursor(NULL, IDC_ARROW);
    vtn.hIcon = NULL;
    vtn.hIconSm = NULL;
    vtn.hInstance = ins;
    vtn.lpfnWndProc = winProc;
    vtn.lpszClassName = app;
    vtn.lpszMenuName = NULL;
    vtn.style = CS_HREDRAW | CS_VREDRAW;

    if(!RegisterClassEx(&vtn)){
        MessageBox(HWND_DESKTOP, "Error al crear la clase", "Error", MB_OK);
    }
    configSocket();
    HANDLE thread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL);
    MSG msj;

    /*Creating the main window*/
    ventana = CreateWindow(app,"McCAIN",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,1250,300, HWND_DESKTOP, NULL, ins, NULL);

    /*Creating the labels*/
    label1 = CreateWindow("STATIC","Output 1:",WS_CHILD | WS_VISIBLE, 20,30,80,20,ventana,NULL,ins,NULL);
    label2 = CreateWindow("STATIC","Output 2:",WS_CHILD | WS_VISIBLE, 20,50,80,20,ventana,NULL,ins,NULL);
    /*Creating the arrays of labels*/
    for(j = 0; j < 2; j++){
        for(i = 0; i < 54; i++){
            outputs[j][i] = CreateWindow("STATIC" ,"X" ,WS_CHILD|WS_VISIBLE ,100+(i*20) ,30 +(j*20) ,20 ,20 ,ventana ,NULL ,ins ,NULL);
        }
    }
    label3 = CreateWindow("STATIC","Inputs:",WS_CHILD | WS_VISIBLE, 30,80,200,20,ventana,NULL,ins,NULL);
    /*Creating the array of buttons*/
    for(j = 0; j < 5; j++){
        for(i = 0; i < 54; i++){
            inputs[j][i] = CreateWindow("BUTTON" ,"0" ,WS_CHILD|WS_VISIBLE ,95+(i*20) ,70 +(j*20) ,20 ,20 ,ventana ,NULL ,ins ,NULL);
            inputData[j][i] = 0;
        }
    }

    /*Exception that shows a message if there's a problem creating the window*/
    if(!ventana){
        MessageBox(HWND_DESKTOP, "Error al crear la ventana", "Error", MB_OK | MB_ICONERROR);
    }

    /*Invoking the window*/
    ShowWindow(ventana, SW_SHOWNORMAL);

    /*Loop for the window's events*/
    while(GetMessage(&msj,0,0,0)){
        DispatchMessage(&msj);
    }
    return (int) msj.wParam;
}
/*End of the main function*/


/*Function that validates the frame*/
int validateFrame()
{
    uint8_t reply[40]={0x01,131,177,0x0,0x0,0x0,5,0x0,0x10,0x18};

    /*Command for the outputs*/
    if(frame[1] == 0x83 && frame[2] == 0x37){
        if((frame[0] == 1) || (frame[0]) == 2){
            updateLabel(frame[0]/2);
        }
        responUDP(reply);
        return 1;
    }

    /*Command for the inputs*/
    if(frame[1] == 0x83 && frame[2] == 0x34){
        if((frame[0] >= 9) && (frame[0] <= 13)){
                printf("\nEl valor que se manda por index es: %d", frame[0]-9);
            sendInputs(frame[0]-9);
        }
    }


    return 0;
}

/*Function to set the output in the label*/
void updateLabel(uint8_t index){
    for(j = 0; j<7; j++){
        mask = 0x80;
        for(i = 0; i<8; i++){
            if((i+(j*8)) < 54){
                if(frame[j+3]&mask)
                    SetWindowText(outputs[index][(i+(j*8))],"1");
                else
                    SetWindowText(outputs[index][(i+(j*8))],"0");
                mask >>= 1;
            }
        }
    }
}

/*Function to send the data of the inputs*/
void sendInputs(uint8_t index){
    for(j = 0; j<7; j++){
        mask = 0x80;
        temp = 0;
        for(i = 0; i<8; i++){
            if((i+(8*j)) <54){
                if(inputData[index][(i+(8*j))] == 1)
                    temp|=mask;
                mask >>= 1;
            }
        }
        aux[j] = temp;
    }
    responUDP(aux);
}



