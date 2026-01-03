// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	/*!
	* \brief Iterates over every value of the parameter list
	*
	* \param callback Callback function called with every parameter contained in the list, which can return true to remove the key (or false to keep it)
	*
	* \remark Changing the ParameterList while iterating on it may cause bugs, but querying data is safe.
	*/
	inline void ParameterList::ForEach(const std::function<bool(const ParameterList& list, const std::string& name)>& callback)
	{
		for (auto it = m_parameters.begin(); it != m_parameters.end();)
		{
			if (callback(*this, it->first))
				it = m_parameters.erase(it);
			else
				++it;
		}
	}

	/*!
	* \brief Iterates over every value of the parameter list
	*
	* \param callback Callback function called with every parameter contained in the list
	*
	* \remark Changing the ParameterList while iterating on it may cause bugs, but querying data is safe.
	*/
	inline void ParameterList::ForEach(const std::function<void(const ParameterList& list, const std::string& name)>& callback) const
	{
		for (auto& pair : m_parameters)
			callback(*this, pair.first);
	}
}
