#include "mbed.h"
#include"HEPTA_EPS.h"
#include"HEPTA_SENSOR.h"
#include"HEPTA_COM.h"
#include"HEPTA_CDH.h"

RawSerial pc(USBTX,USBRX,9600);
HEPTA_EPS eps(p16,p26);
HEPTA_SENSOR sensor(p17,
                  p28,p27,0x19,0x69,0x13,
                  p13, p14,p25,p24);
HEPTA_COM com(p9,p10,9600);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
HEPTA_CDH cdh(p5, p6, p7, p8, "sd");
Timer sattime;

//HeptaSatセンサ検証用コード
//コマンド a→LED
//コマンド b→3.3V コンバータON
//コマンド c→3.3V コンバータOFF
//コマンド d→sattime
//コマンド e→SDcard
//コマンド f→battery
//コマンド g→temprature
//コマンド h→9axis
//コマンド i→gps
//コマンド j→camera
//コマンド k→Xbee

int main()
{
    sattime.start();
    int i = 0;
    //float bt,ax,ay,az;
    float ax,ay,az;
    float bt;
    int rcmd=0;
    int cmdflag=0;
    
    while(1) {
        wait(0.5);
        char c=pc.getc();
        pc.printf("get command = %c\r\n",c);
        eps.turn_on_regulator();//turn on 3.3V conveter
        
        if (c == 'a') {//LED
            pc.printf("Command Get a LED\r\n");
            for(int ii = 0; ii < 5; ii++) {
                led1=1;
                led2=1;
                led3=1;
                led4=1;
                wait(1);
                led1=0;
                led2=0;
                led3=0;
                led4=0;
                wait(1);
            }
        }
        
        if (c == 'b') {//3.3V コンバータON
            pc.printf("Command Get b 3.3V on\r\n");
                eps.turn_on_regulator();//turn on 3.3V conveter
                wait(1);
        }
        if (c == 'c') {//3.3V コンバータOFF
            pc.printf("Command Get c 3.3V off\r\n");
            for(int i=0;i<10;i++){
                pc.printf("3.3V turn off\r\n");
                eps.shut_down_regulator();
                wait(0.5);
            }
            pc.printf("3.3V turn on\r\n");                
        }
        if (c == 'd') {//sattime
            pc.printf("Command Get d timer\r\n");
            for(int ii = 0; ii < 10; ii++) {
                pc.printf("sattime=%f\r\n",sattime.read());
            }
        }
        if (c == 'e') {//SDcard
            pc.printf("Command Get e SD write read\r\n");
            pc.printf("Hello world!\r\n");
            char str[100];
            mkdir("/sd/mydir", 0777);
            FILE *fp = fopen("/sd/mydir/test.txt","w");
            if(fp == NULL) {
                error("Could not open file for write\r\n");
                pc.printf("not find sdcards/r/n");
            }
            for(int i=0; i<10; i++)fprintf(fp,"Hello my name is HEPTA!\r\n");
            pc.printf("write ok!!\r\n");
            pc.printf("lets read!!\r\n");
            fclose(fp);
            fp = fopen("/sd/mydir/test.txt","r");
            for(int j = 0; j < 10; j++) {
                fgets(str,100,fp);
                pc.puts(str);
            }
            fclose(fp);
            pc.printf("ok!!\r\n");
        }
        if (c == 'f') {//battery
            pc.printf("Command Get f battery\r\n");
            for(int ii = 0; ii < 10; ii++) {
                eps.vol(&bt);
                pc.printf("V = %f\r\n",bt);
                wait(0.5);
            }
        }
        if (c == 'g') {//temprature
            pc.printf("Command Get g temprature\r\n");
            eps.turn_on_regulator();//turn on 3.3V conveter
            float temp;
            for (int i=0;i<10;i++) {
                sensor.temp_sense(&temp);
                pc.printf("temp = %f\r\n",temp);
                wait(1.0);
            }
        }
        if (c == 'h') {//9axis
            pc.printf("Command Get h mag\r\n");
            
            float mx,my,mz;
            for(int i = 0; i<10; i++) {
                sensor.sen_mag(&mx,&my,&mz);
                pc.printf("mag : %f,%f,%f\r\n",mx,my,mz);
                wait(1.0);
            }
        }
        if (c == 'i') {//gps
            pc.printf("Command Get i gps\r\n");
            sensor.gps_setting();
            pc.printf("GPS Raw Data Mode\r\n");
            for(int i = 0; i<100; i++) {
                pc.putc(sensor.getc());
            }
        }        
        if (c == 'j') {//camera
            pc.printf("Command Get j camera\r\n");
            FILE *dummy = fopen("/sd/dummy.txt","w");
            if(dummy == NULL) {
                error("Could not open file for write\r\n");
            }
            fclose(dummy);
            
            pc.printf("Camera Snapshot Mode\r\n");
            pc.printf("Hit Any Key To Take Picture\r\n");
            while(!pc.readable()) {}
            sensor.Sync();
            sensor.initialize(HeptaCamera_GPS::Baud115200, HeptaCamera_GPS::JpegResolution320x240);
            sensor.test_jpeg_snapshot_picture("/sd/test.jpg");
        }
        if (c == 'k') {//Xbee
            pc.printf("--com mode--com  put command 'k'\r\n");
            com.xbee_receive(&rcmd,&cmdflag);
            if (rcmd == 'k') {
                pc.printf("Command Get k xbee\r\n");
                pc.printf("Command Get %d\r\n",rcmd);
                com.printf("HEPTA Uplink OK\r\n");
                pc.printf("===================\r\n");
                pc.printf("Accel sensing Mode\r\n");
                pc.printf("===================\r\n");
                for(int ii = 0; ii < 10; ii++) {
                    sensor.sen_acc(&ax,&ay,&az);
                    com.printf("acc : %f,%f,%f\r\n",ax,ay,az);
                    wait(1.0);
                }
            }
            com.initialize();
        }
        
    }
}
