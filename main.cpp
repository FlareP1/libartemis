// Main file to test the artemis connection 

#include "ArtTypes.h"
#include "ArtBase.h"
#include "wxSimpleImageStore.h"
#include "wx/datetime.h"

#include <iostream>
class ArtBase;

int main()
{
   bool m_cooledCam=false;
   bool m_warmupCam=false;
   bool m_isConnected=false;
   bool m_hasfifo=false;
   bool status=false;
   float m_timeRemaining=-1.0f;
   bool exposing = false;
   unsigned short ccdw =0;
   unsigned short ccdh =0;

   std::cout << "Starting Main!\n";

   ArtBase *m_ds = new ArtBase();

   std::cout << "ArtBase Constructed " << m_ds << " \n" ;
  
   //Serach for the Camera
   ArtBase::EnumerateArtCameras();
   // simply make an array of camera names to choose from
   // Do not clear contents here ! cameras.clear();

   std::cout << "ArtBase::EnumerateArtCameras() \n" ;
  
   wxArrayString* cameras_ptr = new wxArrayString();
   wxArrayString& cameras = *cameras_ptr;
   
   int num_items =  ArtBase::NumItems();
   if ( num_items >0 ) {
      for(int item=0; item<num_items; item++) {
         const ArtDevice& descr = ArtBase::ArtDevEntry(item);
         // add camera friendly name to list
         std::cout << "Found Camera " << descr.DevName() << "\n";
         cameras.Add(descr.DevName());
      }
   }

   //Device is good and still availible at this point in time
   
   if( num_items >0 )
   {  // set index and connect to the camera corresponding to "camera"
      std::cout << ".";
      m_ds->ControlDevice(0);  // This causes the Device to go missing from teh device list, even though it is still working.
      std::cout << ".";
      
      // Set Default Binning to x1
      m_ds->SetFormat(1);
      //m_ds->SetFormat(m_fmtChange.bin);
      std::cout << ".";
      //m_ds->SetSubframe(m_fmtChange.s.x, m_fmtChange.s.y, m_fmtChange.e.x, m_fmtChange.e.y); // CCD coords
      //m_fmtChange.SetSubframe(wxPoint(1,1), wxPoint(1,1));
      //SetFullFrame();
      std::cout << ".\n";
      //m_timer.Start(100);     // start the cam timer obj with N ms polling
      //bool retVal = StartPreview();
      if (true) {
         
         m_ds->CcdDimension(ccdw, ccdh); // Get CCD Dimesions

         std::cout << "CCD Size W " << ccdw << " H " << ccdh << "\n";
         //m_camCoord->SetCCDSize(ccdw, ccdh); // should be set only once on connect
         m_cooledCam=m_ds->HasCooling();
         std::cout << "HasCooling " << m_cooledCam << "\n";
         m_warmupCam=m_ds->HasWarmup();
         std::cout << "HasWarmup " << m_warmupCam << "\n";
         m_hasfifo=m_ds->HasFifo();
         std::cout << "HasFifo " << m_hasfifo << "\n";
         m_isConnected = true;
         std::cout << "m_isConnected " << m_isConnected << "\n";
      }
 
      std::cout << "Select a subFrmae for download \n";
      //bool ArtBase::SetSubframe(unsigned short startX, unsigned short startY, unsigned short endX, unsigned short endY)
      status = m_ds->SetSubframe(100,100,ccdw-200,ccdh-200);
      std::cout << status;
      
      std::cout << "Starting Capture \n";
      //bool ArtBase::CaptureImage(bool WXUNUSED(enabled), unsigned long milliseconds)
      status = m_ds->CaptureImage(true, 10000); //1 sec capture
      std::cout << status;

      ARTEMISCAMERASTATE state = m_ds->CaptureStatus();
      std::cout << "Camera State " << status << "\n";

      // Wait for exposure complete
      do
      {
         m_ds->TimeRemaining(exposing , m_timeRemaining);
         std::cout << "TimeRemaining " << m_timeRemaining << " " << exposing << "\n";
         sleep(1);
      } while (exposing == true);

      // Wait for download complete
      do
      {
         m_ds->TimeRemaining(exposing , m_timeRemaining);
         std::cout << "TimeRemaining " << m_timeRemaining << " " << exposing << "\n";
         sleep(1);
      } while (m_timeRemaining > 0.0f);

      state = m_ds->CaptureStatus();
      std::cout << "Camera State " << status << "\n";

      long capResult;
      do
      {
         // allow the camcode to get the samples
         capResult = m_ds->OnCapture(); // allow the camcode to get the samples
         std::cout << "capResult " << capResult << "\n";
         sleep(1);
      } while (capResult != 0);

      wxArtSample *m_pArtSample = new wxArtSample();

      if(capResult == 0  )
      {
         std::cout << "CapturedSample " <<  capResult << " \n";
         m_ds->CapturedSample(*m_pArtSample);  // we _must_ collect the sample if it is ready

      }
      
      
      std::cout << "Image Result " <<  m_pArtSample << " \n";
      //Store the Image
      wxSimpleImageStoreFITS* m_myimageStore = nullptr;
      int xx = ccdw;
      int yy = ccdh;
      m_myimageStore = new wxSimpleImageStoreFITS();
      m_myimageStore->Create(wxT("/home/astroberry/images/"), EVideoSampleFormat::EVF_YP16, EVideoConversion::EVC_Y16, wxSize(xx,yy), 1);
      wxDateTime now = wxDateTime::UNow().ToUTC();
     
      if(m_pArtSample != nullptr)
      {
         float ccdTemp =0.0f;
         m_myimageStore->AddYP16(m_pArtSample->SampleSize(),
                        m_pArtSample->SampleYPtr(), m_pArtSample->SampleByteSizeY(),
                        now, m_pArtSample->ExposureTime(), ccdTemp);
      }

      //m_ds->AbortExposure();
   }
   
   m_ds->DropControlledDevice(); // drop cam object

   sleep(1);
 
   // Delete ArtBase
   delete(m_ds);

   return 0;
}
