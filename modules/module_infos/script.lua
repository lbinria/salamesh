function draw_gui()
	-- imgui.Begin("Foot_bar")
	-- imgui.Text(app.navigation_path_string)
	-- imgui.End()

	if app.has_models then

		if imgui.Button("Filter !!!") then 
			filter()
		end

	end

end

function filter()
	local tri_model = app.model:as_tri()
	local fa = FacetAttributeDouble.new()
	
	fa:bind("_filter", tri_model.surface_attributes, tri_model.mesh)

	for i=0,1000 do 
		fa[i] = 1.0
	end
	
	app.model:set_filter(ElementKind.FACETS_ELT, true)
end