#ifndef ICETRAY_TRANSACTIONALDATABASECLIENT_H
#define ICETRAY_TRANSACTIONALDATABASECLIENT_H

#include "basicDataClient.h"
#include "connection.h"
#include "connection_fwd.h"
#include <c++11Helpers.h>
#include <exception>
#include <memory>
#include <optional>
#include <resourcePool.impl.h>
#include <string>
#include <utility>
#include <visibility.h>
// IWYU pragma: no_include <map>

namespace IceTray {
	class SqlSource;

	template<typename Connection> class DLL_PUBLIC TransactionalDatabaseClient : public BasicDatabaseClient {
	public:
		explicit TransactionalDatabaseClient(Connection db_) : db(std::move(db_))
		{
			db->beginTx();
		}

		virtual ~TransactionalDatabaseClient()
		{
			if (std::uncaught_exceptions()) {
				try {
					db->rollbackTx();
				}
				catch (...) {
				}
			}
			else {
				db->commitTx();
			}
		}

		SPECIAL_MEMBERS_DEFAULT_MOVE_NO_COPY(TransactionalDatabaseClient);

		using BasicDatabaseClient::fetch;
		using BasicDatabaseClient::modify;

		template<typename Domain, typename... Params>
		inline Domain
		fetch(const SqlSource & sql, const Params &... params)
		{
			return fetch<Domain, Params...>(std::nullopt, sql, params...);
		}

		template<typename Domain, typename... Params>
		inline Domain
		fetch(const std::optional<std::string> & typeIdCol, const SqlSource & sql, const Params &... params)
		{
			return fetch<Domain, Params...>(db.get(), typeIdCol, sql, params...);
		}

		template<typename... Params>
		inline auto
		modify(const SqlSource & sql, const Params &... params)
		{
			return modify(db.get(), sql, params...);
		}

	protected:
		Connection db;
	};
	extern template class TransactionalDatabaseClient<DB::ConnectionPtr>;
	extern template class TransactionalDatabaseClient<AdHoc::ResourceHandle<DB::Connection>>;
}

#endif
