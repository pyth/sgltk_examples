from conan import ConanFile

class SgltkExamplesRecipe(ConanFile):
	name = "sgltk_examples"
	version = "0.1.0"
	settings = "os", "compiler", "build_type", "arch"
	generators = "CMakeDeps", "CMakeToolchain"

	def requirements(self):
		self.requires("sgltk/0.6.1")
		self.requires("glew/2.2.0")
		self.requires("glm/1.0.1")
		self.requires("sdl/2.28.3")
		self.requires("sdl_image/2.8.2")
		self.requires("sdl_ttf/2.24.0")
		self.requires("assimp/5.4.3")