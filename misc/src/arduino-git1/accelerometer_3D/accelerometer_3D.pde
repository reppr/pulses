// accelero 3D

#define xAccelerometerPIN 0
#define yAccelerometerPIN 1
#define zAccelerometerPIN 2

#define g1AccelerometerPIN 44
#define g2AccelerometerPIN 42

#define numSAMPLES 10

int delayTime = 0;
int tolerance = 6;


int xAccelValues[numSAMPLES], yAccelValues[numSAMPLES], zAccelValues[numSAMPLES];
int i = 0;
int x, y, z, oldx, oldy, oldz ;
int xsum  = numSAMPLES / 2;  // rounding
int ysum  = numSAMPLES / 2;
int zsum  = numSAMPLES / 2;

void readAccelerometer () {
  xsum -= xAccelValues[i];
  xAccelValues[i] = analogRead(xAccelerometerPIN);
  xsum += xAccelValues[i];
  x = xsum / numSAMPLES;

  ysum -= yAccelValues[i];
  yAccelValues[i] = analogRead(yAccelerometerPIN);
  ysum += yAccelValues[i];
  y = ysum / numSAMPLES;

  zsum -= zAccelValues[i];
  zAccelValues[i] = analogRead(zAccelerometerPIN);
  zsum += zAccelValues[i];
  z = zsum / numSAMPLES;

  i++;
  i %= numSAMPLES;
}
  
void setup () {
  initAccelerometer();

  Serial.begin(9600);
}

void initAccelerometer () {
  int j;

  pinMode(xAccelerometerPIN, INPUT);
  pinMode(yAccelerometerPIN, INPUT);
  pinMode(zAccelerometerPIN, INPUT);

  pinMode(g1AccelerometerPIN, OUTPUT);
  digitalWrite(g1AccelerometerPIN, LOW);
  pinMode(g2AccelerometerPIN, OUTPUT);
  digitalWrite(g2AccelerometerPIN, LOW);

  for (j=0; j<numSAMPLES; j++)
    xAccelValues[j] = yAccelValues[j] = zAccelValues[j] = 0;

  for (j=0; j<numSAMPLES; j++)
    readAccelerometer(); 
}

int absolute(int a) {
  if (a < 0)
    { a = -a; }
  return a;  
}

int isNew () {
  if (absolute(oldx - x) > tolerance )
    return true;
  if (absolute(oldy - y) > tolerance )
    return true;
  if (absolute(oldz - z) > tolerance )
    return true;

  return false;
}

void loop () {
  readAccelerometer();

  if (isNew())
  {    
    Serial.print("x="); Serial.print(x); Serial.print("\t");
    Serial.print("y="); Serial.print(y); Serial.print("\t");
    Serial.print("z="); Serial.print(z); Serial.print("\t");
    Serial.println("");

    oldx = x; oldy = y; oldz = z;

    if (delayTime)
      delay(delayTime);
  }
}

