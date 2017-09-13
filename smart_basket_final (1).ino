#include<SoftwareSerial.h>
#define MAXFRUIT 5

#define bread 2
#define rread 3//load cell reading pins
//identity bit,pin from rack to identify the vegetable rack
#define b 7
//Security states
#define REST 0
#define NORMAL 1
#define PICKUP 2
#define REPLACING 3
#define ABSENCE 4 
#define MISPLACED 5
#define GUIDE 6
#define IMBALANCE 7

#define IMB_THRESH 40

struct security
{
  int secstate;
  long picuptime;
  long montime;
}sec;

char state[]="idle";
char c1[15],c2[15],c;
int i=0,m,j=-1;   //i is reference for appended string,m is the rack no. detected,j is the reference for the no. of items.
int rate,w,wei;

//SoftwareSerial Serial(0,1);//rx,tx 

String fruit[]={"---","apple","orange"};
int price[]={0,40,50};//price per kg

void beep()
{
  analogWrite(10,128);
  delay(2000);
  analogWrite(10,0);
}

struct purchase
{
 char custname[150];//customer name
 char custid[15];//customer id
 int fid[MAXFRUIT];//fruit id
 float w[MAXFRUIT];//weight in kg
 float cost[MAXFRUIT];//total cost
 int count;
}bill;


void billadd()
{
  m=convert();  

  if(m){  
  j = m;
  
  Serial.println("");
  Serial.println("");
  
  w=analogRead(bread);
  wei=((w-850)/4);//weight
  rate=wei*price[m];
  if(bill.fid[j]==0)
    bill.count++;
  bill.fid[j]=m;
  bill.w[j]+=wei;
  bill.cost[j]+=rate;

  Serial.println("The following item has been added to your bill");  
  Serial.println("");

  Serial.print("Item name : ");
  Serial.println(fruit[m]);
  Serial.print("Price per kg : ");
  Serial.println(price[m]);
  Serial.print("Weight measured: ");
  Serial.println(wei);
  Serial.print("Rate of the weight measured: ");
  Serial.println(rate);

  Serial.println("");
  } 
  else beep();
}


void billremove()
{
  m=convert();
  if(m){  
  j = m;
  
  Serial.println("");
  Serial.println("");
  //Discounting item  
  {
  if(bill.fid[j] >0)
    {
      w=analogRead(bread);
      wei=((w-850)/4);//weight
      rate=wei*price[m];
      bill.w[j]-=wei;
      bill.cost[j]-=rate;
      if(bill.w[j]==0)
      {     
        bill.count--;
        bill.fid[j] = 0;
        bill.cost[j] = 0;
      }

  Serial.println("The following item has been removed from your bill");
  Serial.println("");
  Serial.print("Item name : ");
  Serial.println(fruit[m]);
  Serial.print("Price per kg : ");
  Serial.println(price[m]);
  Serial.print("Weight measured: ");
  Serial.println(wei);
  Serial.print("Rate of the weight measured: ");
  Serial.println(rate);
  Serial.println("");
    } 
  else
    {
      beep();
      Serial.println("You have not billed this item yet.");
    }
  }
  }
  else
    beep();
  }


void summary()
{
Serial.println("");
Serial.println("");
Serial.println("Overall Summary : ");  
Serial.println("");
Serial.println("It.No Name   Cost/Kg Qty.(Kg) Price");
int temp=0;
int itmno = 0;
for(temp=0;temp<MAXFRUIT;temp++)
{
  if(bill.fid[temp]){
Serial.print((++itmno)); Serial.print("    ");Serial.print(fruit[bill.fid[temp]]); Serial.print("  "); Serial.print(price[bill.fid[temp]]); Serial.print("      ");
Serial.print(bill.w[temp]); Serial.print("   "); Serial.print(bill.cost[temp]);
Serial.println();
}

Serial.println("");
Serial.print("Total number of items billed: "); Serial.println(bill.count);
Serial.println();
}

}

int convert()
{
int z;
if((analogRead(bread)<=5)||(analogRead(rread)<=5))
{
  z=0;
  Serial.print("Rack reading is ");
  Serial.println(analogRead(rread));
}
else if((digitalRead(b)==LOW))
{
  z=1;
  Serial.print("Rack reading is ");
  Serial.println(analogRead(rread));
}
else if((digitalRead(b)==HIGH))
{
  z=2;
Serial.print("Rack reading is ");
Serial.println(analogRead(rread));
}
return z;
}

void setup() 
{

Serial.begin(9600);

//rack id
pinMode(b,INPUT);

  Serial.println("Command list : ");
  Serial.println("1) add ");
  Serial.println("2) remove ");
  Serial.println("3) summary ");
  Serial.println("4) payment ");

  sec.secstate = 0;
}

void loop() 
{

//Serial.listen();//refresh bluetooth buffer
i = 0;
while(strcmp(state,"idle")==0)
{ 
   
  while(Serial.available() && c !=10)
  {
     c = Serial.read();
     delay(10);
//     Serial.println("string adding: ");
//      Serial.write(c);
     c1[i++]=c;
    
  }
  c=0;
  c1[i]='\0';//ending string
  i=0;

//Serial.println(c1);
if(strcmp(c1,"add")==0)
  {
    c1[1]='v';
    strcpy(state,"add");
  }
else if(strcmp(c1,"cancel")==0)//cancelling the order
 { 
    c1[1]='v';
    Serial.println("Your order is being cancelled....");
    strcpy(state,"idle");
    delay(3500);
    loop();
 }
// else 
// {
//   Serial.println("string not matching.");
//   Serial.println(c1);
// }
 
}

//gathering details of customer
Serial.println("Enter your good name Sir/Madam");
while(!Serial.available());

i=0;

while(Serial.available()&&c !=10)
  {
    delay(10); 
    c = Serial.read();
     bill.custname[i++]=c;    
  }
  c=0;
  bill.custname[i]='\0';//ending string
  i=0;
  
Serial.println("Setting up your cart for shopping...");
for(i = 0; i<MAXFRUIT;i++)
{
  bill.cost[i] =0;
  bill.w[i] = 0;
  bill.fid[i] = 0;
}
bill.count = 0;

Serial.print("HI ");
Serial.println(bill.custname);

Serial.println("Enter your ID");
while(!Serial.available());

while(Serial.available() && c !=10)
  {
    delay(10);
     c = Serial.read();
     bill.custid[i++]=c;  
  }
  c=0;
  bill.custid[i]='\0';//ending string
  i=0;
Serial.print("Thank you, your ID is: ");
Serial.println(bill.custid);

while(strcmp(state,"add")==0)
{

m=convert();
Serial.println("");

Serial.print("FRUIT :");
Serial.println(fruit[m]);
Serial.print("price per kg:");
Serial.println(price[m]);
w=analogRead(bread);
wei=w; //((w-137)/3.2);//weight
Serial.print("Weight measured: ");
Serial.println(wei);
rate=wei*price[m];
Serial.print("Rate of the weight measured: ");
Serial.println(rate);

delay(1500);

c1[1]='v';//to make this character array innvalid
i=0;  
while(Serial.available() && c !=10)
  {
     c = Serial.read();
     delay(10);
     c1[i++]=c;
    
  }
  c=0;
  c1[i]='\0';//ending string
  i=0;

  if(strcmp(c1,"add")==0)
  {
    
    billadd();   
    strcpy(state,"add");
    c1[1]='v';
    summary();
    Serial.println("type anything to proceed....");
    while(!Serial.available());
    while(Serial.available())
    {
      c=Serial.read();
      delay(10);
    }
  }
  else if(strcmp(c1,"remove")==0)
  {
    if(bill.count>=0)
      {
         billremove();
         Serial.println("Your item is removed from the bill.");
         summary();
          Serial.println("type anything to proceed....");
          while(!Serial.available());
          while(Serial.available())
            {
              c=Serial.read();
              delay(10);
            }
      }
   else
        Serial.println("You don't have any item added to the bill yet...");
        
        Serial.println("type anything to proceed....");
          while(!Serial.available());
          while(Serial.available())
            {
              c=Serial.read();
              delay(10);
            }
        
          strcpy(state,"add"); 
          c1[1]='v';
  }
  
  else if(strcmp(c1,"payment")==0)
  {
  summary();
  strcpy(state,"payment");
  c1[1]='v';
  }
  
   else if(strcmp(c1,"summary")==0)
  {
   summary();
    Serial.println("type anything to proceed....");
          while(!Serial.available());
          while(Serial.available())
            {
              c=Serial.read();
              delay(10);
            }
   strcpy(state,"add");
   c1[1]='v';
  }
  else if(strcmp(c1,"cancel")==0)//cancelling the order
 { 
   if(bill.count == 0)
    {
    Serial.println("Your order is being cancelled....");
    strcpy(state,"idle");
    }
    else
    {
      Serial.println("Replace all items before you cancel your order.");
      strcpy(state,"add");
    }
    c1[1]='v';
    delay(3500);
    loop();
 }

}
if(bill.count)
{
  Serial.println("Please enter your mode of payment: ");
  
  i=0;
  while(!Serial.available());
  while(Serial.available() && c !=10)
    {
       c = Serial.read();
       delay(10);
       c2[i++]=c;
    }
    c=0;
    c2[i]='\0';//ending string
    i=0;
    
  Serial.print("Your mode of payment is : ");
  Serial.println(c2);
  c2[1]='v';
  Serial.println("KINDLY UNPAIR YOUR DEVICE");
  
  Serial.println("Thank you, Please visit again");
  
  strcpy(state,"idle");   //changed to initial state
}
else
  {
    Serial.println("Your cart is empty. Do you wish to continue shopping?  [y(default)/n]");
    while(!Serial.available());
    c = 0;
    char cha;
    while(Serial.available()){ cha = c; c = Serial.read(); }
    if(cha == 'n' || cha == 'N')
      {
        strcpy(state,"idle");
        Serial.println("Do visit again.");
      }
    else
      {
        strcpy(state,"add");
      }
  } 
}
