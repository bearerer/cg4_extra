
// \file Camera.h
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

#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include <QVector4D>
#include <QVector3D>

//! OpenGL helper class
/**
 * Camera representation for OpenGL.
 *
 * This class provides the basic functionality for an OpenGL camera placed on
 * a sphere around a certain point of interest.
 */
class Camera
{
 public:

  //---------------------------------------------------------------------------
  //! \name                     Instantiation
  //! \{
  //---------------------------------------------------------------------------

  //! Constructor
  Camera(double polar=0.0, double azimut=0.0, double distance=1.0);

  //! Destructor
  ~Camera(void);

  //! Setup the camera.
  void setup(void);

  //!\}------------------------------------------------------------------------


  //---------------------------------------------------------------------------
  //! \name                     Data Access
  //! \{
  //---------------------------------------------------------------------------

//  void updateAzimut(float louie_angle, float player_angle, bool flies_over_top, float next_angle, int frames_for_turn_over);

  //! Set pitch angle.
  void setPolar(double polar);

  //! Set rotation angle
  void setAzimut(double azimut);

  //! Set look at center
  void setLookAt(int i, double pos) { m_lookAt[i] = pos; update(); }

  //! Set camera distance.
  void setDistance(double distance);


  //! Get pitch angle.
  double polar(void) { return m_polar; }

  //! Get rotation angle.
  double azimut(void) { return m_azimut; }

  //! Get camera position.
  double position(int i) { return m_position[i]; }

  //! Get look at center.
  double lookAt(int i) { return m_lookAt[i]; }

  //! Get up vector.
  double up(int i) { return m_up[i]; }

  //! Get camera distance.
  double distance(void) { return m_distance; }

  void setPosition(double x, double y, double z);
  void setUpVector(double x, double y, double z);
  void setLookAtVector(double x, double y, double z);

  //!\}------------------------------------------------------------------------

   //! Update position.
  void update(void);
//  void initAngles(float ABP); // APB = angle between players
//  void setTransit(int trans, QVector4D camera_pos, QVector4D look_at);
//  void calculateTransit(QVector4D look_at);
//  int curr_transit_frames, transit_frames;

  float lastY() const;
  void setLastY(float lastY);

  float lastX() const;
  void setLastX(float lastX);

  float maxDistance() const;
  void setMaxDistance(float maxDistance);

protected:


  double m_polar;         //!< vertical angle
  double m_azimut;        //!< horizontal angle
  double m_distance;      //!< camera distance to viewpoint

  double m_position[3];   //!< camera position
  double m_lookAt[3];     //!< viewpoint
  double m_up[3];
private:
//    float saved_player_angle, saved_next_angle;
    float _lastX, _lastY;
    float _maxDistance;

//    float threshold_prev, threshold_next, angle_between_players, remaining_angle, factor;
//    float roundToNextPlayerAngle(float angle);
//    float calcAngle(float moving_angle, float fixed_angle);
//    float round(float number);

//    float transit_start_louie, transit_start_azimut;
    QVector3D curr_pos, curr_look_at, target_pos, target_look_at, diff_pos, diff_look_at;
};

#endif // CAMERA_H
