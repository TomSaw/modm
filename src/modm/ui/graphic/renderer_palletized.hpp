void drawBlind(const shape::HLine& hline) {
	if constexpr (PalleteType::Dim == X) {
		// Draw pallete-wise
		auto span = array2dT::rowspan(hline.start.y());
		auto iter = span.begin() + hline.start.x() / PalleteType::size;
		auto rear = span.begin() + hline.end_x / PalleteType::size; // OPTIMIZE Use simplified end sentinel

		PalleteType temp = this->pallete;
		temp<<= ((hline.start.x()) % PalleteType::size);

		// TODO Repetive code
		if(iter != rear) {
			*iter = temp;
			temp = this->pallete;
			++iter;
		}
		
		for(iter; iter != rear; ++iter)
			*iter = temp;

		// FIXME MASK out end
		temp = this->pallete;
		temp>>= PalleteType::size - (hline.end_x % PalleteType::size);
		*iter = temp;
	}
	else // PalleteType::Dim == Y
	{
		// OPTIMIZE implement subspan for iterable_colinear / iterable_perpendicular ?
		auto span = rowspan(hline.start.y());
		for(auto iter = span.begin() + hline.start.x(); iter != span.begin() + hline.end_x + 1; ++iter)
			*iter = this->color;
	}
}

void drawBlind(const shape::VLine& vline) {
	if constexpr (PalleteType::Dim == Y) {
		// Draw pallete-wise
		auto span = array2dT::colspan(vline.start.x());
		auto iter = span.begin() + vline.start.y() / PalleteType::size;
		auto rear = span.begin() + vline.end_y / PalleteType::size; // OPTIMIZE Use simplified end sentinel

		PalleteType temp = this->pallete;
		temp<<= ((vline.start.y()) % PalleteType::size);

		// TODO Repetive code
		if(iter != rear) {
			*iter = temp;
			temp = this->pallete;
			++iter;
		}
		
		for(iter; iter != rear; ++iter)
			*iter = temp;

		// TODO MASK out end
		temp = this->pallete;
		temp>>= PalleteType::size - (vline.end_y % PalleteType::size);
		*iter = temp;
	}
	else // PalleteType::Dim == X
	{
		// OPTIMIZE implement subspan for iterable_colinear / iterable_perpendicular ?
		auto span = colspan(vline.start.x());
		auto iter = span.begin() + vline.start.y();
		const auto end = span.begin() + vline.end_y + 1; // OPTIMIZE Use simplified end sentinel

		for(iter; iter != end; ++iter)
			*iter = this->color;
	}
}