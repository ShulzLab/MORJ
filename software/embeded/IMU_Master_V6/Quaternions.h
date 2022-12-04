#pragma once
#include <ICM_20948.h>
#include <ArduinoJson.h>

struct EulerAngles {

public :
    float roll = 0;
    float pitch = 0;
    float yaw = 0;

    EulerAngles(){}
};

struct Accelerometer{
  public :
  float acc_x;
  float acc_y;
  float acc_z;

  void load_measures(icm_20948_DMP_data_t* data){
    if ((data->header & DMP_header_bitmap_Accel) > 0) // Check the packet contains Accel data
    {
      acc_x = (float)data->Raw_Accel.Data.X; // Extract the raw accelerometer data
      acc_y = (float)data->Raw_Accel.Data.Y;
      acc_z = (float)data->Raw_Accel.Data.Z;
    }
  }
};

struct Gyroscope{
  float gyr_x;
  float gyr_y;
  float gyr_z;
  void load_measures(icm_20948_DMP_data_t* data){
    if ((data->header & DMP_header_bitmap_Gyro) > 0) // Check the packet contains gyro data
    {
      gyr_x = (float)data->Raw_Gyro.Data.X; // Extract the raw accelerometer data
      gyr_y = (float)data->Raw_Gyro.Data.Y;
      gyr_z = (float)data->Raw_Gyro.Data.Z;
    }
  }
};

struct Magnetometer{
  float mag_x;
  float mag_y;
  float mag_z;
  void load_measures(icm_20948_DMP_data_t* data){
    if ((data->header & DMP_header_bitmap_Compass) > 0) // Check the packet contains mag data
    {
      mag_x = (float)data->Compass.Data.X; // Extract the raw accelerometer data
      mag_y = (float)data->Compass.Data.Y;
      mag_z = (float)data->Compass.Data.Z;
    }
  }
};

class Quaternion {
    // Timothe's homemade class to better handle quaternion processing without having to deal with 
    // arrays of 4 values constantly and implementing a ton of separate functions taking them as arguments
public:

    double w = 1;// Q0 is the real part, also called w sometimes online.
    double x = 0;// Q1 is the imaginay part relative to i. Sometimes also called x.  
    double y = 0; // Q2 is the imaginay part relative to j. Sometimes also called y.  
    double z = 0;// Q3 is the imaginay part relative to k. Sometimes also called z.  
    //by default, the quaternion here [1 0 0 0] is at deg. 0 on all axes of rotations.
    double accuracy = 0;

    String object_name;

    Quaternion() {
        //the constructor method. Here empty
    }

    Quaternion(String l_object_name) {
        object_name = l_object_name;
    }

    void commut_product(Quaternion* r) {
        // Multiplying two quaternions together implies some set of developpements as follows :
        // Here we multiply qn, the quaternion components of this class (self in python) 
        // with the quaternion object as argument, r (wich also has components as r.qn)
        // formula : https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/code/index.htm
        w = (r->w * w) - (r->x * x) - (r->y * y) - (r->z * z);
        x = (r->w * x) + (r->x * w) + (r->y * z) - (r->z * y);
        y = (r->w * y) - (r->x * z) + (r->y * w) + (r->z * x);
        z = (r->w * z) + (r->x * y) - (r->y * x) + (r->z * w);
    }  

    void product(Quaternion* r) {
        // Multiplying two quaternions together implies some set of developpements as follows :
        // Here we multiply qn, the quaternion components of this class (self in python) 
        // with the quaternion object as argument, r (wich also has components as r.qn)
        // formula : https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/code/index.htm
        //q=q0+iq1+jq2+kq3
        //r=r0+ir1+jr2+kr3
        //product is : t=q×r=t0+it1+jt2+kt3
        //where 
        //t0=(r0q0−r1q1−r2q2−r3q3)  
        //t1=(r0q1+r1q0−r2q3+r3q2)
        //t2=(r0q2+r1q3+r2q0−r3q1)
        //t3=(r0q3−r1q2+r2q1+r3q0)
        double lw = w;
        double lx = x;
        double ly = y;
        double lz = z;
        w = r->w * lw - r->x * lx - r->y * ly - r->z * lz;
        x = r->w * lx + r->x * lw - r->y * lz + r->z * ly;
        y = r->w * ly + r->x * lz + r->y * lw - r->z * lx;
        z = r->w * lz - r->x * ly + r->y * lx + r->z * lw;
    } 

    void rotate_by(Quaternion* q) {
        // "this" is p
        //To rotate p of q, the formula is rp = q.p.-q (not commutative)
        Quaternion* inv_q = new Quaternion();
        inv_q->take_value_of(q);
        inv_q->conjugate(); // -q
        Quaternion* l_q = new Quaternion();
        l_q->take_value_of(q);// q
        
        l_q->product(this);// q . p
        l_q->product(inv_q);// (q . p) . -q
        
        x = l_q->x;//copy result into current instance
        y = l_q->y;
        z = l_q->z;
        w = l_q->w;
        delete l_q;
        delete inv_q;
    }
    
    void take_value_of(Quaternion* source) {
        //to copy attributes of source quaternion instance as argument in the current instance.
        w = source->w;
        x = source->x;
        y = source->y;
        z = source->z;

        accuracy = source->accuracy;
    }

    void normalize() {
        //All possible values of a quaternion are not valid. To make sure the waternion is valid, we normalize it in that way
        //Any operation that produces a quaternion will need to be normalized because floating - point precession errors will cause it to not be unit length.
        //It is a kludge to drag the quaternion back to the unit 3-sphere, from wich it drifts for reasons mentionned above, involving floating point precision.
        //All the standard formulae for using a quaternion to rotate/transform a vector or to generate a rotation/transformation matrix implicitly assume the quaternion is normalized.
        //The errors that result from using an unnormalized quaternion are proportional to the square of the quaternion's magnitude. Quadratic error growth is something best avoided.
        //Thus, we must normalize after every operation.
        double l_norm = norm();
        l_norm = 1.0 / l_norm; // inverse of length
        w = w * l_norm;// we actually divide by length each component
        x = x * l_norm;
        y = y * l_norm;
        z = z * l_norm;
    }

    void conjugate() {
        //The conjugate of a quaternion is q* = ( q0, −q1, −q2, −q3 ) 
        x = -x;
        y = -y;
        z = -z;
        //w keeps it's sign
    }

    void inverse(){
        //The inverse of a quaternion is equation q-1 = q* / |q|²
        double l_snorm = square_norm();
        //w keeps it's sign
        x = -x/(l_snorm);
        y = -y/(l_snorm);
        z = -z/(l_snorm);     
    }

    double norm() {// "length" of the quaterion
        //If normalized, a quaternion should be of length 1
        double length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2));
        return length;
    }

    double square_norm() {// "length" of the quaterion
        //If normalized, a quaternion should be of length 1
        double length = pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2);
        return length;
    }

    void generate() {
        x = random_float();
        y = random_float();
        z = random_float();

        norm_scale();
    }

    void norm_scale() {
        float scale = 1 / (pow(x, 2) + pow(y, 2) + pow(z, 2));
        x = x * scale;
        y = y * scale;
        z = z * scale;
        w = sqrt(1 - (pow(x, 2) + pow(y, 2) + pow(z, 2)));
        normalize();
    }

    void generate_drift() {
        float rate_of_change = 20.0;
        x += random_float() / rate_of_change;
        y += random_float() / rate_of_change;
        z += random_float() / rate_of_change;
        
        //w = sqrt(1 - ((x * x) + (y * y) + (z * z)));
        norm_scale();
    }

    float random_float() {
        return ((float) random(-1000, 1000)) / 1000 ;
    }

    void load_measures(icm_20948_DMP_data_t* data) {
        
        if ((data->header & DMP_header_bitmap_Quat9) > 0){// If have asked for orientation data from 9 axess in the setup
          x = data->Quat9.Data.Q1 / 1073741824.0;// The quaternion data is scaled by 2^30. (to be able to fit a 1 "unit" quaternion from integer values)
          y = data->Quat9.Data.Q2 / 1073741824.0;
          z = data->Quat9.Data.Q3 / 1073741824.0;
          accuracy = data->Quat9.Data.Accuracy;
        }
        else if ((data->header & DMP_header_bitmap_Quat6) > 0){// If we have asked for orientation data from 6 axes
          x = data->Quat6.Data.Q1 / 1073741824.0;
          y = data->Quat6.Data.Q2 / 1073741824.0;
          z = data->Quat6.Data.Q3 / 1073741824.0;
          accuracy = 0;
        }
        else {
          return;
        }
        // Real part value is computed from this equation: Q0^2 + Q1^2 + Q2^2 + Q3^2 = 1.
        // In case of drift, the sum will not add to 1, therefore, quaternion data need to be corrected with right bias values.
        w = sqrt(1 - ((x * x) + (y * y) + (z * z)));
        if (isnan(w)) {
            w = 0;
        }
    }

    EulerAngles* to_euler_angles(){
        EulerAngles* angles = new EulerAngles();
        angles->roll = roll();
        angles->pitch = pitch();
        angles->yaw = yaw();
        return angles;
    }

    float roll() {//(x-axis rotation)
        double q2sqr = y * y;
        double t0 = +2.0 * (w * x + y * z);
        double t1 = +1.0 - 2.0 * (x * x + q2sqr);
        float roll = atan2(t0, t1) * 180.0 / PI;
        return roll;
    }

    float pitch() {//(y-axis rotation)
        double t2 = +2.0 * (w * y - z * x);
        t2 = t2 > 1.0 ? 1.0 : t2;
        t2 = t2 < -1.0 ? -1.0 : t2;
        float pitch = asin(t2) * 180.0 / PI;
        return pitch;
    }

    float yaw() {//(z-axis rotation)
        double q2sqr = y * y;
        double t3 = +2.0 * (w * z + x * y);
        double t4 = +1.0 - 2.0 * (q2sqr + z * z);
        float yaw = atan2(t3, t4) * 180.0 / PI;
        return yaw;
    }

    bool is_zero() {
        if (x != 0 || y != 0 || z != 0) {
            return false;
        }
        return true;
    }

    //SERIAL_PORT.println(data.Quat9.Data.Accuracy);
    void send_as_json(Stream* SERIAL_PORT) {
        double l_roll, l_pitch, l_yaw;
        l_roll = roll();
        l_pitch = pitch();
        l_yaw = yaw();
        
        SERIAL_PORT->print(F("{\"name\": \""));  SERIAL_PORT->print(object_name); SERIAL_PORT->print(F("\", \"content\":{"));
        SERIAL_PORT->print(F("\"quat_w\":")); SERIAL_PORT->print(w, 5);
        SERIAL_PORT->print(F(", \"quat_x\":"));SERIAL_PORT->print(x, 5);
        SERIAL_PORT->print(F(", \"quat_y\":"));SERIAL_PORT->print(y, 5);
        SERIAL_PORT->print(F(", \"quat_z\":"));SERIAL_PORT->print(z, 5);
        SERIAL_PORT->print(F(", \"accur\":")); SERIAL_PORT->print(accuracy, 3);
        SERIAL_PORT->print(F(", \"norm\":")); SERIAL_PORT->print(norm(), 3);
        SERIAL_PORT->print(F(", \"yaw\":")); SERIAL_PORT->print(l_yaw, 3);
        SERIAL_PORT->print(F(", \"pitch\":")); SERIAL_PORT->print(l_pitch, 3);
        SERIAL_PORT->print(F(", \"roll\":")); SERIAL_PORT->print(l_roll, 3);
        SERIAL_PORT->println(F("}}"));
    }

    void into_json_document(const char* key ,JsonDocument& outputDoc) {
        JsonArray quaternion_components = outputDoc.createNestedArray(key);
        quaternion_components.add(w);
        quaternion_components.add(x);
        quaternion_components.add(y);
        quaternion_components.add(z);
    }
    void into_json_object(const char* key, JsonObject& outputobj) {
        JsonArray quaternion_components = outputobj.createNestedArray(key);
        quaternion_components.add(w);
        quaternion_components.add(x);
        quaternion_components.add(y);
        quaternion_components.add(z);
    }
};
