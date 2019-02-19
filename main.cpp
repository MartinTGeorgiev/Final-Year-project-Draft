#include "mbed.h"

DigitalOut tr1(p5);
DigitalIn  echo1(p6);

DigitalOut tr2(p7);
DigitalIn  echo2(p8);

DigitalOut tr3(p9);
DigitalIn  echo3(p10);

DigitalOut tr4(p11);
DigitalIn  echo4(p12);

DigitalOut ledg(LED1);
DigitalOut ledr(LED2);

InterruptIn b1(p30);

Timer sonar1;
Timer sonar2;
Timer sonar3;
Timer sonar4;
Timer Space;

bool enough = false;
int distance1 = 0;
int distance2 = 0;
int distance3 = 0;
int distance4 = 0;
int correction1 = 0;    //sensor 1 correction value
int correction2 = 0;    //sensor 2 correction value
int correction3 = 0;    //sensor 3 correction value
int correction4 = 0;    //sensor 4 correction value

bool interrupt = 0;
bool fail = 0;
bool success = 0;

float s1= 0, s2 = 0, s3 = 0, s4 = 0; //Distances from sensors

float carL = 34;  //Vehicle length in cm
float carW = 8;   //Vehicle width in cm
float FOS = 4; // Factor of safety in cm
float distAB = 60.2; // distance from point A to point B in cm
float distPO = 30.11; // distance from point 'p' to point 'o' in cm
float distOQ = 44.6; // distance from point 'o' to point 'q' in cm
float spaceL = 0; // parking space Length in cm
int timer = 0;  //measured in seconds
int StageNum = 1;
float carSpeed = 10; //cm per second
float distTraveled = 0; //distance traveled in cm
float minSafeDist = 3; //minimum safe distance for sensors in cm

void StageOne();
void StageTwo();
void StageThree();

void flip();
void sensorsInit();
float sensor1();
float sensor2();
float sensor3();
float sensor4();

int main() {
    
    sensorsInit();
    ledg = 0;
    ledr = 0;
    
      while(1){
        b1.rise(&flip);
        StageNum = 1;
        
        //cout<<"To initiate self-parking procedure press Button: ";
        Space.reset();

        do{

            while((interrupt == 1)&&(fail == 0)&&(success == 0)){

                distTraveled = 0;
                //use data from sensors s1 and s2
                switch(StageNum){

                    case 1: StageOne(); break;
                    case 2: StageTwo(); break;
                    case 3: StageThree(); break;
                }


            }

        }while((fail == 0)&&(interrupt == 1)&&(success ==0));

        if(fail ==1){

            ledr = 1;
            ledg = 0;

        }

        if(interrupt == 0){

            ledr = 1;
            ledg = 0;

        }
        if(success == 1){

            ledr = 0;
            ledg = 1;
        }
    }
}

void sensorsInit(){
    
    sonar1.reset();
// measure actual software polling timer delays
// delay used later in time correction
// start timer
    sonar1.start();
// min software polling delay to read echo pin
    while (echo1==2) {};
// stop timer
    sonar1.stop();
// read timer
    correction1 = sonar1.read_us();
    //printf("Approximate software overhead timer delay is %d uS\n\r",correction);

    sonar2.reset();
// measure actual software polling timer delays
// delay used later in time correction
// start timer
    sonar2.start();
// min software polling delay to read echo pin
    while (echo2==2) {};
// stop timer
    sonar2.stop();
// read timer
    correction2 = sonar2.read_us();
    //printf("Approximate software overhead timer delay is %d uS\n\r",correction);
    
    // measure actual software polling timer delays
// delay used later in time correction
// start timer
    sonar3.start();
// min software polling delay to read echo pin
    while (echo3==2) {};
// stop timer
    sonar3.stop();
// read timer
    correction3 = sonar3.read_us();
    //printf("Approximate software overhead timer delay is %d uS\n\r",correction);

    sonar4.reset();
// measure actual software polling timer delays
// delay used later in time correction
// start timer
    sonar4.start();
// min software polling delay to read echo pin
    while (echo4==2) {};
// stop timer
    sonar4.stop();
// read timer
    correction4 = sonar4.read_us();
    //printf("Approximate software overhead timer delay is %d uS\n\r",correction); 
    
    // measure actual software polling timer delays
// delay used later in time correction

}


float sensor1(){
    
// trigger sonar to send a ping
        tr1 = 1;
        sonar1.reset();
        wait_us(10.0);
        tr1 = 0;
//wait for echo high
        while (echo1==0) {};
//echo high, so start timer
        sonar1.start();
//wait for echo low
        while (echo1==1) {};
//stop timer and read value
        sonar1.stop();
//subtract software overhead timer delay and scale to cm
        distance1 = (sonar1.read_us()-correction1)/58.0;
        
        return distance1;        
}

float sensor2(){
// trigger sonar to send a ping
        tr2 = 1;
        sonar2.reset();
        wait_us(10.0);
        tr2 = 0;
//wait for echo high
        while (echo2==0) {};
//echo high, so start timer
        sonar2.start();
//wait for echo low
        while (echo2==1) {};
//stop timer and read value
        sonar2.stop();
//subtract software overhead timer delay and scale to cm
        distance2 = (sonar2.read_us()-correction2)/58.0;
    
        return distance2;
}
    
float sensor3(){
    
// trigger sonar to send a ping
        tr3 = 1;
        sonar3.reset();
        wait_us(10.0);
        tr3 = 0;
//wait for echo high
        while (echo3==0) {};
//echo high, so start timer
        sonar3.start();
//wait for echo low
        while (echo3==1) {};
//stop timer and read value
        sonar3.stop();
//subtract software overhead timer delay and scale to cm
        distance3 = (sonar3.read_us()-correction3)/58.0;
        
        return distance3;
}

float sensor4(){

        // trigger sonar to send a ping
        tr4 = 1;
        sonar4.reset();
        wait_us(10.0);
        tr4 = 0;
//wait for echo high
        while (echo4==0) {};
//echo high, so start timer
        sonar4.start();
//wait for echo low
        while (echo4==1) {};
//stop timer and read value
        sonar4.stop();
//subtract software overhead timer delay and scale to cm
        distance4 = (sonar4.read_us()-correction4)/58.0;
        
        return distance4;
}

void StageOne(){

    bool s = 0;

    do{
        //vehicle starts moving

        if(sensor1() <= minSafeDist){

            fail = 1;
        }

        else{

            if(s == 0){

                if(sensor2() >= (carW + FOS)){
                        
                    if(Space.read_us() == 0){
                        
                        Space.start();
                    }
                
                    spaceL = (Space.read_us() / 1000000) * carSpeed;
                    distTraveled = spaceL;
                }

                else {

                    if (spaceL >= distAB){
                            
                        //cout<<"\n Space Large enough to park."<<endl;
                        s = 1;
                        ledg = 1;
                        ledr = 0;
                    }

                    else{

                        //cout<<"\nSpace not large enough to park. Continue Moving forward"<<endl;
                        spaceL =0;
                        distTraveled =0;
                        ledr = 1;
                        ledg = 0;
                        Space.stop();
                        Space.reset();
                    }
                }
            }
            
            else {

                distTraveled = carSpeed * (Space.read_us()*1000000);
            }            
            //cout<<"\n spaceL = "<<spaceL<<" distTraveled = "<<distTraveled<<endl;
        }

    }while((distTraveled < (distAB+carL))&&(fail == 0)&&(interrupt == 1));

    if((fail == 0)&&(interrupt == 1)){
            
        //cout<<"\nReached point B, initiating Stage Two."<<endl;
        ledg = 0;
        StageNum = 2;
        Space.stop();
        Space.reset();
        //cout<< "\nDistance traveled so far: "<<distTraveled<<endl;
    }
}

void StageTwo(){
    
    //vehicle stops moving
    //cout<<"Vehicle stops and changes steering angle to 116 degrees"<<endl;
    //cout<<"Vehicle starts moving backwards while using sensors s4 and s3"<<endl;
    do{

        if((sensor3()<= minSafeDist)||(sensor4() <= minSafeDist)){

            fail = 1;
        }
        else{

            if(Space.read_us() == 0){
                    
                        Space.start();
            }
            
            distTraveled = (Space.read_us() / 1000000) * carSpeed;
            //cout<<"\nDistance traveled is: "<<distTraveled<<endl;
        }
        
    }while((distTraveled < distPO)&&(fail == 0)&&(interrupt == 1));

    if((fail == 0)&&(interrupt == 1)){
        //cout<<"\nReached point o, vehicle stopped initiating Stage Two."<<endl;
        StageNum = 3;
        Space.stop();
        Space.reset();
    }

}

void StageThree(){
    //cout<<"Vehicle stops and changes steering angle to 164 degrees"<<endl;
    //cout<<"Vehicle starts moving backwards while using sensors s4 and s3"<<endl;

    distTraveled = 0;

    do{
        /*
        cout<<"\n Distance from sensor s2: ";
        cin>>s;
        cout<<"\nDistance from sensor s4: ";
        cin>>s4;
        */
        
        if((sensor2()<= minSafeDist)||(sensor4() <= minSafeDist)){

            fail = 1;
        }
        else{

            if(Space.read_us() == 0){
                    
                        Space.start();
            }
            
            distTraveled = timer * carSpeed;

            //cout<<"\nDistance traveled is: "<<distTraveled<<endl;
        }

    }while((distTraveled < (distOQ))&&(fail == 0)&&(interrupt == 1));

    if((fail == 0)&&(interrupt == 1)){

        //cout<<"\nReached point q, vehicle stopped into parking position."<<endl;
        Space.stop();
        Space.reset();
        success = 1;
    }
}


void flip(){    
    
    interrupt = !interrupt;    
}