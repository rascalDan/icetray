#ifndef ABSTRACTDATABASECLIENT_H
#define ABSTRACTDATABASECLIENT_H

#include <connectionPool.h>
#include <db/sqlSelectDeserializer.h>
#include <slicer/slicer.h>
#include <cache.h>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <visibility.h>

namespace IceTray {
class DLL_PUBLIC AbstractDatabaseClient {
	public:
		AbstractDatabaseClient(boost::shared_ptr<AdHoc::ResourcePool<DB::Connection>> d);
		typedef std::vector<std::size_t> CacheKey;
		typedef boost::shared_ptr<boost::any> CacheItem;

		template<typename Domain, typename Sql, typename ... Params>
		Domain inline fetch(const Params & ... params)
		{
			CacheKey key;
			key.reserve(sizeof...(Params) + 2);
			key.push_back(Sql::hash);
			key.push_back(typeid(Domain).hash_code());
			keyPushParams(key, params...);
			if (auto cached = cache.get(key)) {
				return boost::any_cast<Domain>(**cached);
			}
			auto c = db->get();
			auto s = DB::SelectCommandPtr(c->newSelectCommand(Sql::sql));
			bind(0, s.get(), params...);
			Domain d = Slicer::DeserializeAny<Slicer::SqlSelectDeserializer, Domain>(*s);
			s.reset();
			c.release();
			cache.add(key, CacheItem(new boost::any(d)), time(NULL) + 40);
			return d;
		}

		template<typename Param, typename ... Params>
		static void inline bind(int offset, DB::Command * cmd, const Param & p, const Params & ... params)
		{
			bind1(offset, cmd, p);
			bind(offset + 1, cmd, params...);
		}

		static void bind(int offset, DB::Command * cmd);

		template<typename Param>
		static void bind1(int offset, DB::Command * cmd, const Param & p);

		template<typename Param, typename ... Params>
		static void inline keyPushParams(CacheKey & k, const Param & p, const Params & ... params)
		{
			k.push_back(std::hash<Param>()(p));
			keyPushParams(k, params...);
		}

		static void keyPushParams(CacheKey &);

		boost::shared_ptr<AdHoc::ResourcePool<DB::Connection>> db;
		AdHoc::Cache<CacheItem, CacheKey> cache;
};
}

#endif

