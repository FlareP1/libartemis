#include "wxSimpleImageStore.h"

// Stream things
#include <wx/wfstream.h>

// FITS things
#include "fits/wxFitsOutputStream.h"
#include "wxBMP2Fits.h"
#include "wxYP162Fits.h"

/// Implement the generic Image Store

wxSimpleImageStore::wxSimpleImageStore()
: m_readyForCapture(false)
, m_fps(1)
{
}

/// Implement the FITS Image Store
wxSimpleImageStoreFITS::wxSimpleImageStoreFITS()
: wxSimpleImageStore()
, m_outStream(NULL)
, m_fitsStream(NULL)
, m_imageCount(0)
{
}

wxSimpleImageStoreFITS::~wxSimpleImageStoreFITS()
{
   this->Close();
}

bool wxSimpleImageStoreFITS::Create(const wxString& dirPath,
                        EVideoSampleFormat WXUNUSED(sampleFormat), EVideoConversion   WXUNUSED(sampleConversion),
                        const wxSize& frameSize, unsigned short fps)
{
   if (m_fitsStream) {
      ::fprintf(stderr,"Image Store - FITS Create");
      ::fprintf(stderr,"Ooops - software error!");
      ::fprintf(stderr,"Software error in: %s, Line >> %u",__FILE__, __LINE__ );
      return false;   // ERROR EXIT - already created
   }
   m_storeDIR = dirPath;
   m_readyForCapture = false;

   // create new buffer
   m_frameSize = frameSize;
   
   //wxNamePolicyContainer* cpolicy = wxF()->getNamePolicyContainer();

   m_storeName.Clear();
   m_storeName.AssignDir(dirPath);
   if ( ! m_storeName.IsDirWritable() ) {
      ::fprintf(stderr,"Image Store - FITS Create\n");
      ::fprintf(stderr,"Cannot write to folder or it does not exist!\n");
      ::fprintf(stderr,"%s\n",m_storeName.GetFullPath());
      return false;  // ERROR EXIT cannot write to dir or it does not exist
   }
   //m_storeName.AppendDir(wxT("/home/astroberry/images"));  // this is a folder name

   m_fps = fps;
   m_imageCount = 0;
   m_readyForCapture = m_storeName.Mkdir(0777, wxPATH_MKDIR_FULL);
   if (m_readyForCapture)
      /* cpolicy->CommitName() */;


   return m_readyForCapture;
}


void wxSimpleImageStoreFITS::Close()
{

   m_frameSize = wxSize(0,0);
   m_readyForCapture = false;
   m_storeName.Clear();
   m_fps = 1;
   m_imageCount = 0;
}


bool wxSimpleImageStoreFITS::AddBMP(const PBITMAPINFOHEADER pBmpHeader, const wxUint8* pBmpData,
                              size_t sampleSize, wxDateTime dateObs, float expoSeconds, float temperature)
{
   // sanity
   if ( (!pBmpHeader) || (!pBmpData) || (!sampleSize) ) {
      ::fprintf(stderr,"Image Store - FITS AddBMP\n");
      ::fprintf(stderr,"Ooops - software error!\n");
      ::fprintf(stderr,"Software error in: %s, Line >> %u",__FILE__, __LINE__ );

      return false;
   }

   //wxNamePolicyFile* fpolicy = wxF()->getNamePolicyFile();
   m_imageCount++;

   wxBMP2Fits fits; // the FITS format handler
   // setup buffer and dump BGR bitmap into the file
   fits.AddBMP(pBmpHeader, pBmpData);
   fits.SetDateObs(dateObs); //BM:20090620 - added
   fits.SetExposureSeconds(expoSeconds);  // FITS needs exposure time in header
   fits.SetCCDTemperature(temperature);   // FITS needs ccd temp in header
   fits.SetFrameNo(m_imageCount);// FITS needs frame number in header
   fits.KeywordListRef() = m_kwdList;     // FITS can have more user keywords in header
   fits.Finalize();

   wxString filename = dateObs.Format(wxT("%y-%m-%d__%H-%M-%S"), wxDateTime::UTC );
   filename = wxT("MyBMPImage_") + filename;
   wxFileName fname;
   fname.AssignDir(m_storeName.GetPath());
   fname.SetName(filename);
   fname.SetExt(wxT("fits"));
  
   m_outStream = new wxFFileOutputStream(fname.GetFullPath());
   if (!m_outStream) {
      ::fprintf(stderr,"Image Store - FITS AddBMP");
      ::fprintf(stderr,"Ooops - Cannot create FITS file!\n");
      ::fprintf(stderr,"%s\n",fname.GetFullPath());
      ::fprintf(stderr,"Cannot create error in: %s, Line >> %u\n",__FILE__, __LINE__ );
      
      return false; // ERROR EXIT cannot create outfile
   }
   m_fitsStream = new wxFitsOutputStream(*m_outStream);
   if (!m_fitsStream) {
      ::fprintf(stderr,"Image Store - FITS AddBMP\n");
      ::fprintf(stderr,"Ooops - Cannot create FITS stream!\n");
      ::fprintf(stderr,"Cannot create error in: %s, Line >> %u\n",__FILE__, __LINE__ );

      delete m_outStream;
      return false; // ERROR EXIT cannot create outfile
   }

   m_fitsStream->Write(fits);
   bool retVal = (m_fitsStream->GetLastError()==wxSTREAM_NO_ERROR);
   m_fitsStream->Close();
   if (m_fitsStream) {
      delete m_fitsStream; m_fitsStream = NULL;
      delete m_outStream;  m_outStream = NULL;
   }
   if (retVal)
    /* fpolicy->CommitName() */;

   return retVal;
}

// add a YP16 (Y channel plane 16bit)
bool wxSimpleImageStoreFITS::AddYP16(const wxSize& frameSize,
                               const wxUint16* pYData, size_t sampleSize,
                               wxDateTime dateObs, float expoSeconds, float temperature)
{
   // sanity
   if ( (!pYData) || (!sampleSize) ) {
      wxArrayString messages;
      ::fprintf(stderr,"Image Store - FITS AddYP16\n");
      ::fprintf(stderr,"Ooops - software error!\n");
      ::fprintf(stderr,"Software error in: %s, Line >> %u\n",__FILE__, __LINE__ );

      return false;
   }

   //wxNamePolicyFile* fpolicy = wxF()->getNamePolicyFile();
   m_imageCount++;

   wxYP162Fits fits; // the FITS format handler
   // setup buffer and dump BGR bitmap into the file
   fits.AddYP16(frameSize, pYData);
   fits.SetDateObs(dateObs); //BM:20090620 - added
   fits.SetExposureSeconds(expoSeconds);  // FITS needs exposure time in header
   fits.SetCCDTemperature(temperature);   // FITS needs ccd temp in header
   fits.SetFrameNo(m_imageCount);   // FITS needs frame number in header
   fits.KeywordListRef() = m_kwdList;     // FITS can have more user keywords in header
   fits.Finalize();


   wxString filename = dateObs.Format(wxT("%y-%m-%d__%H-%M-%S"), wxDateTime::UTC );
   filename = wxT("MyfitsImage_") + filename;
   wxFileName fname;
   fname.AssignDir(m_storeName.GetPath());
   fname.SetName(filename);
   fname.SetExt(wxT("fits"));

   m_outStream = new wxFFileOutputStream(fname.GetFullPath());
   if (!m_outStream) {
      ::fprintf(stderr,"Image Store - FITS AddYP16\n");
      ::fprintf(stderr,"Ooops - Cannot create FITS file!");
      ::fprintf(stderr,"%s\n",fname.GetFullPath());
      ::fprintf(stderr,"Cannot create error in: %s, Line >> %u",__FILE__, __LINE__ );

      return false; // ERROR EXIT cannot create outfile
   }
   m_fitsStream = new wxFitsOutputStream(*m_outStream);
   if (!m_fitsStream) {
      ::fprintf(stderr,"Image Store - FITS AddYP16\n");
      ::fprintf(stderr,"Ooops - Cannot create FITS file!");
      ::fprintf(stderr,"Cannot create error in: %s, Line >> %u",__FILE__, __LINE__ );

      delete m_outStream;
      return false; // ERROR EXIT cannot create outfile
   }

   m_fitsStream->Write(fits);
   bool retVal = (m_fitsStream->GetLastError()==wxSTREAM_NO_ERROR);
   m_fitsStream->Close();
   if (m_fitsStream) {
      delete m_fitsStream; m_fitsStream = NULL;
      delete m_outStream;  m_outStream = NULL;
   }
   if (retVal) 
      /*fpolicy->CommitName()*/;

   return retVal;
}


