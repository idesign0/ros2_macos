// Copyright 2018 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TEST_RUNNER__EXECUTE_PROCESS_HPP_
#define TEST_RUNNER__EXECUTE_PROCESS_HPP_

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace test_runner
{

namespace impl
{

#if defined(_WIN32)
int execute_process_win32(const std::vector<std::string> & commands);
#else
int execute_process_unix(const std::vector<std::string> & commands);
#endif

}  // namespace impl

/// Execute a process and return the return code when it exits.
int
execute_process(const std::vector<std::string> & commands)
{
#if defined(_WIN32)
  return impl::execute_process_win32(commands);
#else
  return impl::execute_process_unix(commands);
#endif
}

#if defined(_WIN32)

int impl::execute_process_win32(const std::vector<std::string> & commands)
{
  int exit_code = -1;

  std::string command_str = "";
  for (auto command : commands) {
    command_str += command + " ";
  }
  if (commands.size()) {
    // remove trailing " "
    command_str = command_str.substr(0, command_str.size() - 1);
  }
  LPSTR lpstr_command = _strdup(command_str.c_str());

  STARTUPINFO info = {sizeof(info)};
  PROCESS_INFORMATION processInfo;
  if (CreateProcess(
      NULL, lpstr_command,
      NULL, NULL, TRUE, 0, NULL, NULL,
      &info, &processInfo))
  {
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    DWORD dw_exit_code;
    GetExitCodeProcess(processInfo.hProcess, &dw_exit_code);
    exit_code = dw_exit_code;

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
  }

  free(lpstr_command);

  return exit_code;
}

#else

int impl::execute_process_unix(const std::vector<std::string> & commands)
{
  int exit_code = 0;

  pid_t pid = fork();
  if (-1 == pid) {
    throw std::runtime_error("failed to fork()");
  } else if (0 == pid) {
    // child
    // executable to be run (found on PATH)
    const char * cmd = commands[0].data();
    // argv for new process (need non-const char *'s), including program name in slot 0
    std::vector<char *> arguments;
    auto it = commands.cbegin();
    for (; it != commands.cend(); ++it) {
      // dup strings to get non-const, free'd after execvp
      arguments.push_back(strdup(it->data()));
    }
    arguments.push_back(nullptr);  // explicit nullptr to tell execvp where to stop
    int ret = execvp(cmd, arguments.data());
    for (auto str : arguments) {
      free(str);
      str = nullptr;
    }
    if (-1 == ret) {
      fprintf(stderr, "failed to call execvp(): %s\n", strerror(errno));
    }
    _exit(127);
  } else {
    // parent
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      throw std::runtime_error("failed to waitpid()");
    } else {
      if (WIFSIGNALED(status)) {
        exit_code = -WTERMSIG(status);
      } else if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
      } else if (WIFSTOPPED(status)) {
        exit_code = -WSTOPSIG(status);
      } else {
        // should not happen
        throw std::runtime_error("unknown child exit status");
      }
    }
  }

  return exit_code;
}

#endif

}  // namespace test_runner

#endif  // TEST_RUNNER__EXECUTE_PROCESS_HPP_
