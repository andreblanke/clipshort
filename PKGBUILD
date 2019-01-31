# Maintainer: Andre Blanke <blanke.and+aur@gmail.com>
_pkgname="clipshort"
pkgname="$_pkgname-git"
pkgver=r9.45e9b5f
pkgrel=1
pkgdesc="Shorten URLs copied to the clipboard"
arch=("i686" "x86_64")
url="https://github.com/andreblanke/clipshort"
license=("Apache")
depends=("libclipboard" "libx11")
makedepends=("cmake" "extra-cmake-modules" "git")
provides=("$_pkgname")
conflicts=("$_pkgname")
source=("$pkgname::git+$url.git")
sha256sums=("SKIP")

pkgver() {
    cd "$pkgname"
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
    cd "$pkgname"
    cmake . -DCMAKE_INSTALL_PREFIX="/usr"
}

package() {
    cd "$pkgname"
    make DESTDIR="${pkgdir}" install
}
