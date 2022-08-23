#include <regex>
#include <fstream>
#include "../../hdr/system/s_fileBootstrap.h"
#include "com_fileMapping.h"

#define EVENT_INOTIFY_THREAD_NAME "inotifyThread"

std::vector<std::string>           fileBootstrapNames{"./server/fileBootstrap.ini", "data/fileBootstrap.ini", "build/fileBootstrap.ini"};
std::string                        g_fileBootstrapFilename{};
std::map<std::string, std::string> fileMapping;
int                                inotifyFileHandle{};
int                                inotifyWatchDescriptor{};

droidFileMapping serverFileMapping{};

//--------------------------------------------------------------------------------------------------------------------
//
// Find the basefile file and load in index to file mapping
bool initFileMapping ()
//--------------------------------------------------------------------------------------------------------------------
{
	if (!serverFileMapping.getBootstrapFilename (fileBootstrapNames))
	{
		serverMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString ("%s", serverFileMapping.getLastError ().c_str ()));
		return false;
	}

	if (!serverFileMapping.getFileMappingsFromFile ())
	{
		serverMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString ("%s", serverFileMapping.getLastError ().c_str ()));
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Setup inotify to monitor for changes to files in data directory
bool initFileWatch ()
//--------------------------------------------------------------------------------------------------------------------
{
	inotifyFileHandle = inotify_init ();

	if (inotifyFileHandle < 0)
	{
		s_shutdownWithError ("ERROR: Unable to complete inotify_init().");
		return false;
	}

	inotifyWatchDescriptor = inotify_add_watch (inotifyFileHandle, serverFileMapping.getfileMappedName ("watchDirectory").c_str (), IN_MODIFY | IN_CREATE | IN_DELETE);
	if (inotifyWatchDescriptor < 0)
	{
		s_shutdownWithError (sys_getString ("Unable to add watch to directory [ %s ]", serverFileMapping.getfileMappedName ("watchDirectory").c_str ()));
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Function that is run in thread to send alerts on file changes
int inotifyThreadFunction ([[maybe_unused]]void *param)
//--------------------------------------------------------------------------------------------------------------------
{
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( ( EVENT_SIZE + 16 ) * 1024)

	int  i = 0;
	char buffer[BUF_LEN];

	while (serverThreads.canThreadRun (EVENT_INOTIFY_THREAD_NAME))
	{
		if (serverThreads.isThreadReady (EVENT_INOTIFY_THREAD_NAME))
		{
			SDL_Delay (THREAD_DELAY_MS);

			auto length = read (inotifyFileHandle, buffer, BUF_LEN);

			if (length < 0)
				serverMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, "Read error from inotify event.");

			i = 0;
			while (i < length)
			{
				auto *event = (struct inotify_event *) &buffer[i];
				if (event->len)
				{
					if (event->mask & IN_CREATE)
					{
						if (event->mask & IN_ISDIR)
						{
							printf ("The directory %s was created.\n", event->name);
						}
						else
						{
							printf ("The file %s was created.\n", event->name);
						}
					}
					else if (event->mask & IN_DELETE)
					{
						if (event->mask & IN_ISDIR)
						{
							printf ("The directory %s was deleted.\n", event->name);
						}
						else
						{
							printf ("The file %s was deleted.\n", event->name);
						}
					}
					else if (event->mask & IN_MODIFY)
					{
						if (event->mask & IN_ISDIR)
						{
							printf ("The directory %s was modified.\n", event->name);
						}
						else
						{
							printf ("The file %s was modified.\n", event->name);
						}
					}
				}
				i += EVENT_SIZE + event->len;
			}
		}
	}
	(void) inotify_rm_watch (inotifyFileHandle, inotifyWatchDescriptor);
	(void) close (inotifyFileHandle);

	serverMessage.message (MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_STD_OUT, sys_getString ("Thread [ %s ] has finished.", EVENT_INOTIFY_THREAD_NAME));

	return 1;
}

//--------------------------------------------------------------------------------------------------------------------
//
// Start a thread to run the inotify function
bool startiNotifyThread ()
//--------------------------------------------------------------------------------------------------------------------
{
	if (!initFileWatch ())
	{
		s_shutdownWithError ("Unable to start inotify file watch.");
		return false;
	}

	if (!serverThreads.registerThread (inotifyThreadFunction, EVENT_INOTIFY_THREAD_NAME))
	{
		s_shutdownWithError (sys_getString ("%s\n", serverThreads.getErrorString ().c_str ()));
		return false;
	}

	if (!serverThreads.setThreadRunState (true, EVENT_INOTIFY_THREAD_NAME))
	{
		serverMessage.message(MESSAGE_TARGET_STD_OUT, sys_getString("Unable to set thread run state inotify thread [ %s ]", serverThreads.getErrorString ().c_str()));
		return false;
	}

	if (!serverThreads.setThreadReadyState (true, EVENT_INOTIFY_THREAD_NAME))
	{
		serverMessage.message(MESSAGE_TARGET_STD_OUT, sys_getString("Unable to set thread ready state inotify thread [ %s ]", serverThreads.getErrorString ().c_str()));
		return false;
	}

	return true;
}
