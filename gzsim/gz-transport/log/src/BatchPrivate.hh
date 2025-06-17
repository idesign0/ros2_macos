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
#ifndef GZ_TRANSPORT_LOG_BATCHPRIVATE_HH_
#define GZ_TRANSPORT_LOG_BATCHPRIVATE_HH_

#include <memory>
#include <vector>

#include "gz/transport/log/SqlStatement.hh"
#include "raii-sqlite3.hh"

using namespace ignition::transport;
using namespace ignition::transport::log;

/// \brief Private implementation for Batch
/// \internal
class gz::transport::log::BatchPrivate
{
  /// \brief constructor
  /// \param[in] _db an open sqlite3 database handle wrapper
  /// \param[in] _statements a list of statments to be executed to get messages
  public: explicit BatchPrivate(
      const std::shared_ptr<raii_sqlite3::Database> &_db,
      std::vector<SqlStatement> &&_statements);  // NOLINT(build/c++11)

  /// \brief destructor
  public: ~BatchPrivate();

  /// \brief topic names that should be queried
  public: std::shared_ptr<std::vector<SqlStatement>> statements;

  /// \brief SQLite3 database pointer wrapper
  public: std::shared_ptr<raii_sqlite3::Database> db;
};

#endif
