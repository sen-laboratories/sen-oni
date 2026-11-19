// lodtool2 <out.hvif|out.iom> <in.svg> <base N> <minLOD base> <maxLOD base> <minLOD of the others> ... : several drawings in one icon, each with its levels of detail.
// Per file: file, the number of its first shapes that are "base" (the others are "detail"), the minimum level of detail of the base, the maximum of all its shapes, and the minimum of the detail.
#include <cstdio>
#include <cstdlib>
#include <string>
#include "IconConverter.h"
using namespace haiku;
int main(int argc, char** argv)
{
	if (argc < 7 || (argc - 2) % 5 != 0) { fprintf(stderr, "usage: lodtool2 out in.svg N minBase max minDetail [in.svg ...]\n"); return 1; }
	Icon icon;
	bool first = true;
	for (int a = 2; a + 4 < argc + 0; a += 5) {
		Icon part = IconConverter::Load(argv[a], FORMAT_SVG);
		const size_t base = (size_t)atoi(argv[a + 1]);
		const float minBase = (float)atof(argv[a + 2]), maxAll = (float)atof(argv[a + 3]), minDetail = (float)atof(argv[a + 4]);
		for (size_t i = 0; i < part.shapes.size(); i++) { part.shapes[i].minLOD = i < base ? minBase : minDetail; part.shapes[i].maxLOD = maxAll; }
		printf("%s: %zu shapes, %zu base\n", argv[a], part.shapes.size(), base);
		if (first) { icon = part; first = false; continue; }
		const int styleOffset = (int)icon.styles.size(), pathOffset = (int)icon.paths.size();
		for (const Style& s : part.styles) icon.styles.push_back(s);
		for (const Path& p : part.paths) icon.paths.push_back(p);
		for (Shape s : part.shapes) {
			s.styleIndex += styleOffset;
			for (int& pi : s.pathIndices) pi += pathOffset;
			icon.shapes.push_back(s);
		}
	}
	std::string out = argv[1];
	IconFormat format = out.size() > 4 && out.substr(out.size() - 4) == ".iom" ? FORMAT_IOM : FORMAT_HVIF;
	if (!IconConverter::Save(icon, out, format)) { fprintf(stderr, "save failed: %s\n", IconConverter::GetLastError().c_str()); return 1; }
	printf("%zu shapes in all\n", icon.shapes.size());
	return 0;
}
