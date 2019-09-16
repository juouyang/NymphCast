/*
	av_io.h - Header file for the static AV_IO class.
	
	Revision 0
	
	Notes:
			- 
		
	2019/09/15 - Maya Posch
*/


#ifndef AV_IO_H
#define AV_IO_H


#include <iostream>

#include <vlc/vlc.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mutex.h>


#include <Poco/Condition.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>


#define WIDTH 640
#define HEIGHT 480
 
#define VIDEOWIDTH 320
#define VIDEOHEIGHT 240


struct SDLContext {
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_mutex *mutex;
	int n;
};


struct DataBuffer {
	std::vector<std::string> data;	// The data byte, inside string instances.
	uint64_t size;				// Number of bytes in data vector.
	uint64_t currentIndex;		// The current index into the vector element.
	uint32_t currentSlot;		// The current vector slot we're using.
	uint32_t slotSize;			// Slot size in bytes.
	uint32_t slotBytesLeft;		// Bytes left in the current slot.
	uint32_t numSlots;			// Total number of slots in the data vector.
	uint32_t nextSlot;			// Next slot to fill in the buffer vector.
	uint32_t freeSlots;			// Slots free to write new data into.
	uint32_t buffBytesLeft;		// Number of bytes available for reading in the buffer.
	bool eof;					// Whether End of File for the source file has been reached.
	
	uint64_t buffIndexLow;		// File index at the buffer front.
	uint64_t buffIndexHigh;		// File index at the buffer back.
	Poco::Mutex mutex;
	int activeSession;
	Poco::Condition bufferDelayCondition;
	Poco::Mutex bufferDelayMutex;
	Poco::Condition requestCondition;
	Poco::Mutex requestMutex;
	bool requestInFlight;
};


// --- Globals ---
extern DataBuffer media_buffer;
	
	
class AV_IO : public Poco::Runnable {
	libvlc_media_t *m;	
	libvlc_media_player_t* mp;
	libvlc_instance_t* libvlc;
	static struct SDLContext context;
	//DataBuffer* buffer = 0;
	bool running;
	
	bool init();
	bool vlcInit();
	
	static void* lock(void* data, void** p_pixels);
	static void unlock(void* data, void* id, void* const* p_pixels);
	static void display(void* data, void* id);
	
	static int media_open_cb(void *opaque, void **datap, uint64_t *sizep);
	static ssize_t media_read_cb(void *opaque, unsigned char *buf, size_t len);
	static int media_seek_cb(void* opaque, uint64_t offset);
	static void media_close_cb(void* opaque);
	
public:
	//void setBuffer(DataBuffer* buffer);
	virtual void run();
	void quit();
};


#endif
