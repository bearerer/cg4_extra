
// \file Camera.cpp
// --------------------------------------------------------------------------
// (C) Copyright, 2006
// Computer Graphics Group, University of Siegen, Germany
// All Rights Reserved.
//
// This is unpublished proprietary source code.
// The contents of this file may not be disclosed to third parties,
// copied or duplicated in any form, in whole or in part, without the
// prior written permission of the copyright holder.
// --------------------------------------------------------------------------

#include "camera.h"

#define _USE_MATH_DEFINES 1

#include <float.h>
#include <math.h>
#include <cmath>
#include <QtOpenGL/qgl.h>
#include <QtCore/qmath.h>

#ifdef __APPLE_CC__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <iostream>

//#include <GL/glu.h>
//#include <OpenGL/glu.h> //MAC_OS_X

#define DEG2RAD(alpha) (((alpha)*M_PI)/180.0)
#define RAD2DEG(alpha) (((alpha)*180.0)/M_PI)


/**
 * Constructor.
 *
 * @param polar     Initial polar angle
 * @param azimut    Initial azimut angle
 * @param distance  Initial distance to the coordinate center
 */
Camera::Camera(double polar, double azimut, double distance) :
m_polar(polar), m_azimut(azimut), m_distance(distance)/*, transit_frames(0)*/
{
    m_lookAt[0] = 0.0;
    m_lookAt[1] = 0.0;
    m_lookAt[2] = 0.0;
    m_up[0] = 0.0;
    m_up[1] = 1.0;
    m_up[2] = 0.0;

}


/**
 * Standard destructor.
 */
Camera::~Camera(void)
{
}


//void Camera::initAngles(float ABP){
//    angle_between_players = ABP;
//    threshold_prev = angle_between_players*0.5;
//    threshold_next = -angle_between_players/6;
//    remaining_angle = angle_between_players - fabs(threshold_next) - threshold_prev;
//    factor = angle_between_players / remaining_angle;
//}

/** Update position **/
void Camera::update(void)
{
  m_position[0] = m_lookAt[0] + m_distance*cos(DEG2RAD(m_polar))*sin(DEG2RAD(m_azimut));
  m_position[1] = m_lookAt[1] + m_distance*sin(DEG2RAD(m_polar));
  m_position[2] = m_lookAt[2] + m_distance*cos(DEG2RAD(m_polar))*cos(DEG2RAD(m_azimut));
}


/**
 * OpenGL camera setup during rendering pass.
 */
void Camera::setup(void)
{
  gluLookAt(
    m_position[0], // eyepoint
    m_position[1],
    m_position[2],

    m_lookAt[0], // center
    m_lookAt[1],
    m_lookAt[2],

    m_up[0], // up vector
    m_up[1],
    m_up[2]
  );

//    std::cout  << m_lookAt[0]<< " " << m_lookAt[1]<< " " << m_lookAt[2] << std::endl;
}


/**
 * Set pitch angle.
 */
void Camera::setPolar(double polar)
{
  if (polar >= 90.0)
    m_polar = 89.99;
  else if (polar <= -90.0)
    m_polar = -89.99;
  else
    m_polar = polar;

}


/**
 * Set rotation angle
 */
void Camera::setAzimut(double azimut)
{
  m_azimut = azimut - 360.0*floor(azimut/360.0);

  if (m_azimut < 0.0)
    m_azimut += 360.0;
}


/**
 * Set camera distance.
 */
void Camera::setDistance(double distance) {
  if (distance < FLT_EPSILON)
    distance = FLT_EPSILON;

  m_distance = distance;
  update();
}

void Camera::setPosition(double x, double y, double z){
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}
void Camera::setUpVector(double x, double y, double z){
    m_up[0] = x;
    m_up[1] = y;
    m_up[2] = z;
}

void Camera::setLookAtVector(double x, double y, double z){
    m_lookAt[0] = x;
    m_lookAt[1] = y;
    m_lookAt[2] = z;
}

/**
 * Updates the camera's azimut. If Louie is flying over the top,
 * the camera's position is interpolated between the angle at which
 * Louie started and the angle where he will land again.
 * If he is flying normally, see Camera::calcAngle
 */
//void Camera::updateAzimut(float louie_angle, float player_angle, bool flies_over_top, float next_angle, int frames_for_turn_over){
//    if(flies_over_top){
//        if(player_angle == next_angle)
//            setAzimut(RAD2DEG(player_angle));
//        else{
//            if(saved_player_angle == -1000){
//                saved_player_angle = player_angle;
//                saved_player_angle = calcAngle(louie_angle, player_angle);
//                saved_next_angle = calcAngle(next_angle, roundToNextPlayerAngle(next_angle));
//            }

//            float diff = saved_player_angle-saved_next_angle;
//            while(diff > DEG2RAD(360.0)) diff -= DEG2RAD(360.0);
//            while(diff <= DEG2RAD(0.0)) diff += DEG2RAD(360.0);
//            if(diff < DEG2RAD(180.0)){
//                setAzimut(m_azimut-RAD2DEG(diff/frames_for_turn_over));
//            }
//            else{
//                setAzimut(m_azimut+RAD2DEG(diff/frames_for_turn_over));
//            }
//        }
//    }
//    else{
//        float new_angle = calcAngle(louie_angle, player_angle);
//        setAzimut(RAD2DEG(new_angle));
//        if(saved_player_angle != -1000){
//            saved_player_angle = -1000;
//        }
//    }

//    // Update azimut
//    float distance = 20.0;
//    m_position[0] = distance*cos(DEG2RAD(m_polar))*sin(DEG2RAD(m_azimut));
//    m_position[1] = distance*sin(DEG2RAD(m_polar));
//    m_position[2] = distance*cos(DEG2RAD(m_polar))*cos(DEG2RAD(m_azimut));


//}

//float Camera::round(float number){
//    float truncated = int(number);
//    if(number - truncated > 0.5)
//        return truncated+1;
//    return truncated;
//}

/**
 * Returns the closest player angle to the given angle.
 */
//float Camera::roundToNextPlayerAngle(float angle){
//    float num = this->round(angle/angle_between_players);
//    float new_angle =  angle_between_players*num;
//    if(new_angle >= DEG2RAD(360.0)) new_angle -= DEG2RAD(360.0);
//    return new_angle;
//}

/**
 * Calculates the current azimut.
 * If Louie (moving_angle) has not reached the player (fixed_angle) yet,
 * the player's angle is return (camera stationed, waiting for Louie).
 * If Louie has just passed the player (diff > threshold_next), but is not
 * too far away, the player's angle is returned as well (camera station, looking
 * at Louie's aft section (zu deutsch: Heck). If Louie is in between two players,
 * the camera's azimut is interpolated between the two angles of the players.
 */
//float Camera::calcAngle(float moving_angle, float fixed_angle){
//    float diff = 0.0;
//    if(fixed_angle < angle_between_players){
//        if(moving_angle < angle_between_players){
//            diff = fixed_angle-moving_angle;
//        }
//        else{
//            fixed_angle += DEG2RAD(360.0);
//            diff = fixed_angle-moving_angle;
//        }
//    }
//    else{
//        diff = fixed_angle-moving_angle;
//    }
//    if(diff >= 0 && diff  < threshold_prev)
//        return fixed_angle;
//    else if(diff > threshold_next)
//        return fixed_angle;
//    else{
//        return fixed_angle-(diff-threshold_next)*factor;
//    }

//}
//void printVector(QVector3D vec){
//    std::cout << vec.x() << " " << vec.y() << " "<<  vec.z() << std::endl;
//}
//void Camera::setTransit(int trans, QVector4D camera_pos, QVector4D look_at){

//    curr_transit_frames = trans;
//    transit_frames = trans;
//    target_pos = QVector3D(camera_pos.x(), camera_pos.y(), camera_pos.z());
//    target_look_at = QVector3D(look_at.x(), look_at.y(), look_at.z());

//    curr_pos = QVector3D(m_position[0], m_position[1], m_position[2]);
//    curr_look_at = QVector3D(m_lookAt[0], m_lookAt[1], m_lookAt[2]);
//    diff_pos = (target_pos - curr_pos)/trans;
//    diff_look_at = (target_look_at - curr_look_at)/trans;
//}
//void Camera::calculateTransit(QVector4D look_at){
//    curr_pos += diff_pos;
//    curr_look_at += diff_look_at;
//    setPosition(curr_pos.x(), curr_pos.y(), curr_pos.z());
//    float factor = curr_transit_frames/float(transit_frames);
//    // Exponentially interpolate between the starting look at-vector and the target look at-vector
//    factor = qPow(factor, 10.0);
//    setLookAtVector(curr_look_at.x()*factor + (1-factor)* look_at.x(),
//                    curr_look_at.y()*factor + (1-factor)* look_at.y(),
//                    curr_look_at.z()*factor + (1-factor)* look_at.z());
//    curr_transit_frames--;
//}

float Camera::lastX() const
{
    return _lastX;
}

void Camera::setLastX(float lastX)
{
    _lastX = lastX;
}

float Camera::lastY() const
{
    return _lastY;
}

void Camera::setLastY(float lastY)
{
    _lastY = lastY;
}

float Camera::maxDistance() const
{
    return _maxDistance;
}

void Camera::setMaxDistance(float maxDistance)
{
    _maxDistance = maxDistance;
}
