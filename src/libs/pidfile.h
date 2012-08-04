/**
 * file: pidfile.cpp
 * author: Antonio Messina <antonio.messina@ictp.it>
 * author: Riccardo Murri <riccardo.murri@ictp.it>
 *
 * Interface to the %pidfile class.
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


#ifndef __PIDFILE_H
#define __PIDFILE_H

#include <string>

/** Open and lock a pidfile on construction, 
    unlock and remove it on destruction. */
class pidfile {
  public:
    pidfile(const std::string &filename);
    pidfile(const char * const filename);
    virtual ~pidfile();

    /** Open and lock pidfile, write current process PID to it. */
    void write();

  private:
    /** Pathname to the pidfile. */
    std::string pidfile_path;

    /** File descriptor for locking the pidfile. */
    int pidfile_fd;
};

#endif // __PIDFILE_H