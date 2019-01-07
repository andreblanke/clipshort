pkgname="clipshort-git"
pkgver="1.0.0"
pkgrel="1"
pkgdesc="Shorten URLs copied to the clipboard"
url="https://github.com/andreblanke/clipshort"
arch=("i686" "x86_64")
license=("Apache")
depends=("libclipboard" "libxcb")
makedepends=("cmake" "extra-cmake-modules" "git")
provides=("clipshort")
source=("https://github.com/andreblanke/clipshort")
sha256sums=("SKIP")

build() {
    cd "clipshort"
    cmake . -DCMAKE_INSTALL_PREFIX="/usr"
}

package() {
    cd "clipshort"
    make DESTDIR="${pkgdir}" install
}
