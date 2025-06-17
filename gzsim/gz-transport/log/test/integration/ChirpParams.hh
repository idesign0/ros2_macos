/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef GZ_TRANSPORT_LOG_TEST_INTEGRATION_CHIRPPARAMS_HH_
#define GZ_TRANSPORT_LOG_TEST_INTEGRATION_CHIRPPARAMS_HH_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif
#include <gz/msgs/int32.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <gz/transport/test_config.h>

#include <string>
#include <vector>

#include <gz/transport/Node.hh>


namespace ignition
{
  namespace transport
  {
    namespace log
    {
      namespace test
      {
        /// \brief Parameter used to determine how long the topicChirp_aux
        /// program will wait between emitting message chirps from its topic.
        /// Value is in milliseconds.
        const int DelayBetweenChirps_ms = 1;

        /// \brief Parameter used to determine how long the topicChirp_aux
        /// program will wait (after it advertises) before it begins publishing
        /// its message chirps. Value is in milliseconds.
        const int DelayBeforePublishing_ms = 1000;

        /// \brief This is the message type that will be used by the chirping
        /// topics.
        using ChirpMsgType = gz::msgs::Int32;


        //////////////////////////////////////////////////
        /// \brief Similar to testing::forkAndRun(), except this function
        /// specifically calls the INTEGRATION_topicChirp_aux process and passes
        /// it arguments to determine how it should chirp out messages over its
        /// topics.
        /// \param _topics A list of topic names to chirp on
        /// \param _chirps The number of messages to chirp out. Each message
        /// will count up starting from the value 1 and ending with the value
        /// _chirps.
        /// \return A handle to the process. This can be used with
        /// testing::waitAndCleanupFork().
        testing::forkHandlerType BeginChirps(
            const std::vector<std::string> &_topics,
            const int _chirps,
            const std::string &_partitionName)
        {
          // Set the chirping process name
          const std::string process =
              IGN_TRANSPORT_LOG_BUILD_PATH"/INTEGRATION_topicChirp_aux";

          // Argument list:
          // [0]: Executable name
          // [1]: Partition name
          // [2]: Number of chirps
          // [3]-[N]: Each topic name
          // [N+1]: Null terminator, required by execv
          const std::size_t numArgs = 3 + _topics.size() + 1;

          std::vector<std::string> strArgs;
          strArgs.reserve(numArgs-1);
          strArgs.push_back(process);
          strArgs.push_back(_partitionName);
          strArgs.push_back(std::to_string(_chirps));
          strArgs.insert(strArgs.end(), _topics.begin(), _topics.end());

        #ifdef _MSC_VER
          std::string fullArgs;
          for (std::size_t i = 0; i < strArgs.size(); ++i)
          {
            if (i == 0)
            {
              // Windows prefers quotes around the process name
              fullArgs += "\"";
            }
            else
            {
              fullArgs += " ";
            }

            fullArgs += strArgs[i];

            if (i == 0)
            {
              fullArgs += "\"";
            }
          }

          char * args = new char[fullArgs.size()+1];
          std::snprintf(args, fullArgs.size()+1, "%s", fullArgs.c_str());

          STARTUPINFO info = {sizeof(info)};
          PROCESS_INFORMATION processInfo;

          if (!CreateProcess(nullptr, args, nullptr, nullptr,
                             TRUE, 0, nullptr, nullptr, &info, &processInfo))
          {
            std::cerr << "Error running the chirp process ["
                      << args << "]\n";
          }

          delete[] args;

          return processInfo;
        #else
          // Create a raw char* array to pass to execv
          char * * args = new char*[numArgs];

          // Allocate a char array for each argument and copy the data to it
          for (std::size_t i = 0; i < strArgs.size(); ++i)
          {
            const std::string &arg = strArgs[i];
            args[i] = new char[arg.size()+1];
            std::snprintf(args[i], arg.size()+1, "%s", arg.c_str());
          }

          // The last item in the char array must be a nullptr, according to the
          // documentation of execv
          args[numArgs-1] = nullptr;

          testing::forkHandlerType pid = fork();

          if (pid == 0)
          {
            if (execv(process.c_str(), args) == -1)
            {
              int err = errno;
              std::cerr << "Error running the chirp process [" << err << "]: "
                        << strerror(err) << "\n";
            }
          }

          // Clean up the array of arguments
          for (std::size_t i = 0; i < numArgs; ++i)
          {
            char *arg = args[i];
            delete[] arg;
            arg = nullptr;
          }
          delete[] args;
          args = nullptr;

          return pid;
        #endif
        }
      }
    }
  }
}

#endif
