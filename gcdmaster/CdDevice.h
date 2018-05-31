/*  cdrdao - write audio CD-Rs in disc-at-once mode
 *
 *  Copyright (C) 1998-2001  Andreas Mueller <mueller@daneb.ping.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __CD_DEVICE_H__
#define __CD_DEVICE_H__

#include <gtkmm.h>

#include <vector>
#include <string>

class TocEdit;
class Process;
class ScsiIf;

class CdDevice : public sigc::trackable
{
public:
    enum Status { DEV_READY, DEV_RECORDING, DEV_READING, DEV_WAITING, DEV_BUSY,
                  DEV_NO_DISK, DEV_BLANKING, DEV_FAULT, DEV_UNKNOWN, DEV_LAST };
    enum DeviceType { CD_R = 0, CD_RW = 1, CD_ROM = 2, CD_LAST = 3 };

    enum Action { A_RECORD, A_READ, A_DUPLICATE, A_BLANK, A_NONE };

    CdDevice(const char* dev, const char *vendor, const char *product);
    ~CdDevice();

    char *settingString() const;

    const char *dev() const            { return dev_.c_str(); }
    const char *vendor() const         { return vendor_.c_str(); }
    const char *product() const        { return product_.c_str(); }
    const char *description() const    { return description_.c_str(); }

    Status status() const              { return status_; }
    Process *process() const           { return process_; }

    int exitStatus() const;

    void status(Status);
    int updateStatus();

    Action action() const              { return action_; }

    bool updateProgress(Glib::IOCondition, int fd);

    int autoSelectDriver();

    int driverId() const                 { return driverId_; }
    void driverId(int);

    DeviceType deviceType() const;
    void deviceType(DeviceType);

    unsigned long driverOptions() const;
    void driverOptions(unsigned long);

    bool manuallyConfigured() const       { return manuallyConfigured_; }
    void manuallyConfigured(bool b)       { manuallyConfigured_ = b; }

    bool ejectCd(bool load=false);
    bool loadCd() { return ejectCd(true); }

    bool recordDao(Gtk::Window& parent, TocEdit *, int simulate,
                   int multiSession, int speed, int eject, int reload,
                   int buffer, int overburn);
    void abortDaoRecording();

    int extractDao(Gtk::Window& parent, const char *tocFileName, int correction,
                   int readSubChanMode);
    void abortDaoReading();

    int duplicateDao(Gtk::Window& parent, int simulate, int multiSession,
                     int speed, int eject, int reload, int buffer, int onthefly,
                     int correction, int readSubChanMode, CdDevice *readdev);
    void abortDaoDuplication();

    int blank(Gtk::Window* parent, int fast, int speed, int eject, int reload);
    void abortBlank();
    
    int progressStatusChanged();
    void progress(int *status, int *totalTracks, int *track,
                  int *trackProgress, int *totalProgress,
                  int *bufferFill, int *writerFill) const;
  
    static std::vector<std::string> statusNames;
    static std::vector<std::string> devtypeNames;
    static std::vector<std::string> driverNames;

    static const char *driverName(int id);
    static int driverName2Id(const char *);
    static DeviceType devtypeName2Id(const std::string);

    static void importSettings();
    static void exportSettings();

    static CdDevice *add(const char* scsidev, const char *vendor,
                         const char *product);

    static CdDevice *add(const char *setting);

    static CdDevice *find(const char* dev);
  
    static void scan();

    static void remove(const char* dev);

    static void clear();
    static int count();
    static CdDevice* at(int pos);

    static int updateDeviceStatus();


private:
    std::string dev_; // SCSI device
    std::string vendor_;
    std::string product_;
    std::string description_;

    DeviceType deviceType_;

    int driverId_;
    unsigned long driverOptions_;

    bool manuallyConfigured_;

    ScsiIf *scsiIf_;
    int scsiIfInitFailed_;
    Status status_;

    enum Action action_;

    int exitStatus_;

    int progressStatusChanged_;
    int progressStatus_;
    int progressTotalTracks_;
    int progressTrack_;
    int progressTotal_;
    int progressTrackRelative_;
    int progressBufferFill_;
    int progressWriterFill_;

    Process *process_;

    CdDevice *next_;
    CdDevice *slaveDevice_; // slave device (used when copying etc.)

    void createScsiIf();

    static bool init_;
    static const char *driver_names_[];
    static std::vector<CdDevice*> DEVICE_LIST_;
    static void init();
};

#endif
