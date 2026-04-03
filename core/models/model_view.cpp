#include "model_view.h"

void ModelView::setSelectedAttr(int idx, ColormapLayer layer) {
	selectedAttr[layer] = idx;
	unsetColormaps(layer);

	if (idx < 0)
		return;

	auto attr = model.getAttr(idx);
	auto attrName = attr.name;
	ElementKind kind = attr.kind;

	setColormapAttr(attrName, kind, layer);
	setColormap(kind, layer);
}

void ModelView::resetLayer(ElementKind kind, Layer layer) {
	// Prepare a vector of zeros of the size of the element kind to unset
	std::vector<float> zeros;
	switch (kind) {
		case ElementKind::CELLS_ELT: {
			zeros.resize(model.ncells(), 0.f);
			break;
		}
		case ElementKind::CELL_FACETS_ELT:
		case ElementKind::FACETS_ELT: {
			zeros.resize(model.nfacets(), 0.f);
			break;
		}
		case ElementKind::EDGES_ELT: {
			zeros.resize(model.nhalfedges(), 0.f);
			break;
		}
		case ElementKind::CORNERS_ELT: 
		case ElementKind::CELL_CORNERS_ELT: {
			zeros.resize(model.ncorners(), 0.f);
			break;
		}
		case ElementKind::POINTS_ELT: {
			zeros.resize(model.nverts(), 0.f);
			break;
		}
	}

	// If renderer is rendering this kind of element, set requested layer data to zeros
	for (auto const &[k ,r] : rendererViews) {
		if (r->isRenderElement(kind)) {
			r->setLayer(zeros, layer);
		}
	}
}

void ModelView::setLayer(ElementKind kind, Layer layer, bool update) {

	if (activatedLayers[{layer, kind}] && !update)
		return;

	for (auto const &[k, r] : rendererViews) {
		if (r->isRenderElement(kind)) {
			r->setLayerElement(kind, layer);
		}
	}

	if (update)
		updateLayer(layer, kind);

	activatedLayers[{layer, kind}] = true;
}

void ModelView::unsetLayer(ElementKind kind, Layer layer, bool reset) {
	// Little optimisation, doesn't update data
	// if layer isn't activated, no need to unset
	if (!activatedLayers[{layer, kind}])
		return;
	
	// Set requested layer data to zeros
	if (reset)
		resetLayer(kind, layer);

	for (auto const &[k ,r] : rendererViews) {
		if (r->isRenderElement(kind)) {
			r->setLayerElement(-1, layer); // element -1 means => deactivate layer
		}
	}

	activatedLayers[{layer, kind}] = false;
}