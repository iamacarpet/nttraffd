/**
 * file: pidfile.cpp
 * author: Antonio Messina <antonio.messina@ictp.it>
 * author: Riccardo Murri <riccardo.murri@ictp.it>
 *
 * Implementation of the %pidfile class.
 *
 */
/*
 * Copyright (c) 2005, 2006 Riccardo Murri <riccardo.murri@ictp.it>
 * Copyright (c) 2005, 2006 Antonio Messina <antonio.messina@ictp.it>
 * for the ICTP project EGRID.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#include "pidfile.h"

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


pidfile::pidfile(const std::string &filename)
  : pidfile_path(filename), pidfile_fd(-1)
{
  // nothing else to do
}


pidfile::pidfile(const char * const filename)
  : pidfile_path(filename), pidfile_fd(-1)
{
  // nothing else to do
}


void
pidfile::write()
{
  // open pidfile for writing
  pidfile_fd = open(pidfile_path.c_str(), 
                    O_WRONLY|O_CREAT|O_NOFOLLOW, 0644);
  if (0 > pidfile_fd)
    {
      int err = errno;
      std::ostringstream msg;
      msg << "Cannot open pidfile '" << pidfile_path.c_str() << "': "
          << strerror(err);
      throw std::runtime_error(msg.str());
    }

  // lock pidfile for writing
  int rc = lockf(pidfile_fd, F_TLOCK, 0);
  if (-1 == rc)
    {
      int err = errno;
      std::ostringstream msg;
      msg << "Cannot lock pidfile '" << pidfile_path << "': "
          << strerror(err);
      throw std::runtime_error(msg.str());
    }

  // truncate pidfile at 0 length
  ftruncate(pidfile_fd, 0);

  // write our pid
  try
    {
      std::ofstream pidf(pidfile_path.c_str());
      pidf << getpid();
    }
  catch(std::exception x) {
    std::ostringstream msg;
    msg << "Cannot write pidfile '" << pidfile_path << "': "
        << x.what();
    throw std::runtime_error(msg.str());
  }
}

pidfile::~pidfile()
{
  if(-1 != pidfile_fd)
    {
      // pidfile has been opened and locked
      lockf(pidfile_fd, F_ULOCK, 0);
      close(pidfile_fd);
      unlink(pidfile_path.c_str());
    }
}