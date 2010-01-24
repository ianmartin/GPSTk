// GPSTk wrap for src library

%module gpstk
%{
#include <iostream>
#include <fstream>
#include <valarray>
#include <string>
#include <map>
#include <ios>

%}


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"

#ifdef PYTHON
 //jay had to add
%include "std_ios.i"
#endif

%include "gpstk_AllanDeviation.i"
%include "gpstk_AlmOrbit.i"
%include "gpstk_Antenna.i"
//%include "gpstk_AstronomicalFunctions.i"
%include "gpstk_BaseDistribution.i"
%include "gpstk_BasicFramework.i"
%include "gpstk_BinexFilterOperators.i"
//%include "gpstk_BinUtils.i"					**requires -cpperraswarn on swig compilation
%include "gpstk_BivarStats.i"
%include "gpstk_CheckPRData.i"
%include "gpstk_ClockModel.i"
%include "gpstk_CodeBuffer.i"
%include "gpstk_CommandOptionParser.i"
%include "gpstk_CommonTime.i"
%include "gpstk_convhelp.i"
%include "gpstk_DayTime.i"
%include "gpstk_EllipsoidModel.i"
%include "gpstk_ENUUtil.i"
//%include "gpstk_Epoch.i"
%include "gpstk_exception.i"
%include "gpstk_Expression.i"
%include "gpstk_ExtractC1.i"
%include "gpstk_ExtractData.i"
%include "gpstk_FFData.i"	
%include "gpstk_FFStream.i"
%include "gpstk_FFStreamError.i"
%include "gpstk_FileFilter.i"
%include "gpstk_FileHunter.i"
%include "gpstk_FileSpec.i"
%include "gpstk_FileStore.i"
%include "gpstk_FileUtils.i"
%include "gpstk_GenXSequence.i"
%include "gpstk_geometry.i"
%include "gpstk_GPSAlmanacStore.i"
%include "gpstk_gps_constants.i"
%include "gpstk_GPSZcount.i"
%include "gpstk_icd_200_constants.i"
%include "gpstk_IonoModel.i"
%include "gpstk_IonoModelStore.i"
%include "gpstk_Matrix.i"
%include "gpstk_MiscMath.i"
//%include "gpstk_ModeledPseudorangeBase.i"
%include "gpstk_MoonPosition.i"
%include "gpstk_MOPSWeight.i"
%include "gpstk_NEDUtil.i"
%include "gpstk_ObsEpochMap.i"
%include "gpstk_ObsID.i"
%include "gpstk_ObsRngDev.i"
%include "gpstk_OceanLoading.i"
%include "gpstk_orbitgroup.i"
%include "gpstk_ORDEpoch.i"
%include "gpstk_PCodeConst.i"
%include "gpstk_PoleTides.i"
%include "gpstk_PolyFit.i"
%include "gpstk_Position.i"
//%include "gpstk_PowerSum.i"
%include "gpstk_RACRotation.i"
%include "gpstk_RinexMetFilterOperators.i"		
%include "gpstk_RinexNavFilterOperators.i"
%include "gpstk_RinexObsFilterOperators.i"
%include "gpstk_RinexUtilities.i"
//%include "gpstk_RTFileFrame.i"
//%include "gpstk_RungeKutta4.i"
%include "gpstk_SatID.i"
%include "gpstk_SMODFData.i"
%include "gpstk_SMODFStream.i"
%include "gpstk_SolidTides.i"
%include "gpstk_SourceID.i"
%include "gpstk_SpecialFunctions.i"
%include "gpstk_Stats.i"
%include "gpstk_stl_helpers.i"
%include "gpstk_SunPosition.i"
//%inlucde "gpstk_SVExclusionList.i"
%include "gpstk_SVNumXRef.i"
%include "gpstk_SVPCodeGen.i"
%include "gpstk_TimeConstants.i"
%include "gpstk_TimeConverters.i"
%include "gpstk_TimeNamedFileStream.i"
%include "gpstk_TimeString.i"
%include "gpstk_TimeTag.i"
%include "gpstk_TropModel.i"	
%include "gpstk_TypeID.i"
%include "gpstk_ValidType.i"
%include "gpstk_VectorBase.i"
//%include "gpstk_VectorBaseOperators.i"
//%include "gpstk_VectorOperators.i"
%include "gpstk_WeightBase.i"
%include "gpstk_WxObsMap.i"
%include "gpstk_X1Sequence.i"
%include "gpstk_X2Sequence.i"					 			
%include "gpstk_Zcount.i"
%include "common.i"


//%include "gpstk_ANSITime.i" 				**included in TimeTag.i**
//%include "gpstk_Bancroft.i"				**included in Matrix.i**
//%include "gpstk_BinexData.i" 				**included in FFData.i**
//%include "gpstk_BinexStream.i"			**included in FFStream.i**
//%include "gpstk_BLQDataReader.i"			**included in FFStream.i **
//%include "gpstk_DOP.i"				**included in Matrix.i**
//%inlcude "gpstk_ConfDataReader.i"  			**included in FFStream.i
//%include "gpstk_ExtractCombinationData.i"		**included in ExctractData.i**
//%include "gpstk_ExtractD1.i"				**included in ExctractData.i**
//%include "gpstk_ExtractD2.i"				**included in ExctractData.i**
//%include "gpstk_ExtractL1.i"				**included in ExctractData.i**
//%include "gpstk_ExtractL2.i"				**included in ExctractData.i**
//%include "gpstk_ExtractLC.i"				**included in ExctractData.i**	
//%include "gpstk_ExtractP1.i"				**included in ExctractData.i**
//%include "gpstk_ExtractP2.i"				**included in ExctractData.i**
//%include "gpstk_ExtractPC.i"				**included in ExctractData.i**
//%include "gpstk_FFBinaryStream.i"			**included in FFStream. **
//%include "gpstk_FFTextStream.i"			**included in FFStream.i**
//%include "gpstk_FICAStream.i"				**included in FFStream.i**
//%include "gpstk_FICBase.i"				**included in FFData.i	**
//%include "gpstk_FICFilterOperators.i"  		**included in FFData.i **
//%include "gpstk_FICDataSet.i"  			**included in FFData.i	**
//%include "gpstk_FICHeader.i"				**included in FFData.i	**
//%include "gpstk_FICStream.i"				**included in FFStream **
//%include "gpstk_FICStreamBase.i"			**included in FFStream.i*
//%include "gpstk_FileFilterFrame.i"			**included in FileFilter.i**
//%include "gpstk_FileFilterFrameWithHeader.i"		**included in FileFilter.i**
//%include "gpstk_GaussianDistribution.i"		**included in BaseDistribution.i**
//%include "gpstk_Geodetic.i"   			**included in Position.i**
//%include "gpstk_GeoidModel.i"				**included in WGS84Geoid.i**
//%include "gpstk_GPSEllipsoid.i"			**included in EllipsoidModel.i**
//%include "gpstk_GPSEphemerisStore.i"			**included in XvtStore.i**
//%include "gpstk_GPSGeoid.i"				**included in geoidModel.i**
//%include "gpstk_GPSWeek.i"				**included in TimeTag.i**
//%inlcude "gpstk_GPSWeekSecond.i"			**included in TimeTag.i**
//%include "gpstk_GPSWekkZcount.i"			**included in TimeTag.i**
//%include "gpstk_InOutFramework.i"			**included in BasicFramework**
//%include "gpstk_IonexBase.i"				**included in FFData.i**
//%include "gpstk_IonexData.i" 				**included in FFData.i**
//%include "gpstk_IonexHeader.i" 			**included in FFData.i**
//%include "gpstk_IonexStream.i"			**included in FFStream.i**
//%include "gpstk_IonexStore.i"				**included in FileStore.i**
//%include "gpstk_JulianDate.i"				**included in TimeTag.i**
//%include "gpstk_LinearClockModel.i"			**included in ClockModel.i**
//%include "gpstk_LoopedFramework.i"			**included in BasicFramework.i**
//%include "gpstk_MatrixBase.i"				**included in Matrix.i**
//%include "gpstk_MatrixBaseOperators.i"		**included in Matrix.i
//%inlcude "gpstk_MatrixFunctors.i"			**included in Matrix.i
//%include "gpstk_MJD.i"				**included in TimeTag.i**
//%include "gpstk_MSCBase.i"				**included in FFData.i**
//%include "gpstk_MSCData.i"				**included in FFData.i**
//%include "gpstk_MSCHeader.i"  			**included in FFData.i**
//%include "gpstk_MSCStore.i"				**included in XvtStore.i**
//%include "gpstk_MSCStream.i"				**included in FFStream.i**
//%include "gpstk_ObsClockModel.i"			**included in ClockModel.i**
//%include "gpstk_PRSolution.i"				**included in Matrix.i**
//%include "gpstk_RinexEphemerisStore.i"		**included in GPSEphemerisStore.i**
//%include "gpstk_RinexMetBase.i"			**included in FFData.i**
//%include "gpstk_RinexMetData.i"			**included in FFData.i**
//%include "gpstk_RinexMetHeader.i"			**included in FFData.i**
//%include "gpstk_RinexMetStream.i"			**included in FFStream.i**
//%include "gpstk_RinexNavBase.i"			**included in FFData.i**
//%include "gpstk_RinexNavData.i"			**included in FFData.i**
//%include "gpstk_RinexNavHeader.i"			**included in FFData.i**
//%include "gpstk_RinexNavStream.i"			**included in FFStream.i**
//%include "gpstk_RinexObsBase.i"			**included in FFData.i**
//%include "gpstk_RinexObsData.i"			**included in FFData.i**
//%include "gpstk_RinexObsHeader.i"			**included in FFData.i**
//%include "gpstk_RinexObsID.i"				**included in ObsID.i**
//%include "gpstk_RinexObsStream.i"			**included in FFStream.i**
//%include "gpstk_RinexSatID.i"				**included in SatID.i**
//%include "gpstk_SatDataReader.i"			**included in FFStream.i**
//%include "gpstk_SEMAlmanacStore.i"			**included in GPSAlmanacStore.i**
//%include "gpstk_SEMBase.i"				**included in FFData.i**
//%include "gpstk_SEMData.i"				**included in FFData.i**
//%include "gpstk_SEMHeader.i"				**included in FFData.i**
//%include "gpstk_SEMStream.i"				**included in FFStream.i**
//%include "gpstk_SimpleKalmanFilter.i"			**included in Matrix.i**
//%include "gpstk_SimpleIURAWeight.i"			**included in WeightBase.i**
//%include "gpstk_SP3Base.i"				**included in FFData.i**
//%include "gpstk_SP3Data.i"				**included in FFData.i**
//%include "gpstk_SP3EphemerisStore.i"			**included in FileStore.i**
//%include "gpstk_SP3SatID.i"				**included in SatID.i**
//%include "gpstk_SP3Header.i"				**included in FFData.i**
//%include "gpstk_SP3Stream.i"				**included in FFStream.i**
//%include "gpstk_SolverBase.i"				**included in Matrix.i**
//%include "gpstk_StringUtils.i"			**included in exception.i**
//%include "gpstk_StudentDistribution.i"		**included in BaseDistribution.i**
//%include "gpstk_SystemTime.i"				**included in TimeTag.i**
//%include "gpstk_TabularEphemerisStore.i"		**included in XvtStore.i**
//%include "gpstk_Triple.i" 				**included in Position.i**
//%include "gpstk_UnixTime.i"				**included in TimeTag.i**
//%include "gpstk_WGS84Ellipsoid.i"			**included in EllipsoidModel.i**
//%include "gpstk_WGS84Geoid.i"				**included in GeoidModel.i**
//%inlcude "gpstk_Xvt.i"        			**included in Position.i**
//%inlcude "gpstk_XvtStore.i"				**included in Position.i**
//%inlcude "gpstk_YDSTime.i"				**included in TimeTag**
//%inlcude "gpstk_YumaAlmanacStore.i"			**included in GPSAlmanacStore.i**
//%inlcude "gpstk_YumaBase.i"				**included in FFData.i**
//%inlcude "gpstk_YumaData.i"				**included in FFData.i**
//%inlcude "gpstk_YumaHeader.i"				**included in FFData.i**
//%inlcude "gpstk_YumaStream.i"  			**included in FFStream.i**

