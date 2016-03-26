namespace Tet {
	inline PartComponent::PartComponent(const PartComponent::PartForm<3, 2>& partForm, const std::vector<Ndk::EntityHandle>& partBlocks)
		: form{ partForm },
		blocks{ partBlocks }
	{}
}