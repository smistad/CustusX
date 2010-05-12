/*
 *  sscReconstructAlgorithm.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */

#ifndef SSCRECONSTRUCTALGORITHM_H_
#define SSCRECONSTRUCTALGORITHM_H_

#include <vector>
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"

namespace ssc
{
typedef boost::shared_ptr<class TimedPosition> TimedPositionPtr;
/** Represents one position with timestamp 
 */
class TimedPosition
{
public:
  double mTime;
  Transform3D mPos;
};


typedef boost::shared_ptr<class ReconstructAlgorithm> ReconstructAlgorithmPtr;

/**
 * Abstact interface for reconstruction algorithm. Subclass in order to 
 * implement a reconstruction
 */
class ReconstructAlgorithm
{
public:
  ~ReconstructAlgorithm() {};
  /**
   * \param frameInfo Timetags and positions for the input frames
   * \param frameData The frame data. Assumes that the transfrom is identity.
   * \param outputData [Out] The reconstructed volume. Memory must be allocated in advance.
   */
  virtual void reconstruct(std::vector<TimedPosition> frameInfo, 
                           ImagePtr frameData,
                           ImagePtr outputData,
                           ImagePtr frameMask) = 0;
};

}//namespace
#endif //SSCRECONSTRUCTALGORITHM_H_