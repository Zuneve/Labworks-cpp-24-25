#pragma once

#include <optional>

#include <aggregate_by_key.h>
#include <as_data_flow.h>
#include <as_vector.h>
#include <dir.h>
#include <drop_null_opt.h>
#include <filter.h>
#include <join_result.h>
#include <join.h>
#include <kv.h>
#include <open_files.h>
#include <out.h>
#include <split_expected.h>
#include <split.h>
#include <transform.h>
#include <write.h>


template<typename Range, typename Adapter>
auto operator|(Range&& range, Adapter&& adapter)
{
	return adapter(std::forward<Range>(range));
}
