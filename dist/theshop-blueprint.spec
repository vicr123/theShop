Name:           theshop-blueprint
Version:        1.0
Release:        1%{?dist}
Summary:        Application Shopping Center

License:        GPLv3+
URL:            https://github.com/vicr123/theshop
Source0:        https://github.com/vicr123/theshop/archive/%{version}.tar.gz

%if 0%{?fedora} == 32
BuildRequires:  make qt5-devel the-libs-blueprint-devel libtdesktopenvironment-blueprint-devel libthedesk-blueprint-devel PackageKit-Qt5-devel appstream-qt-devel
Requires:       qt5 the-libs-blueprint libtdesktopenvironment-blueprint libthedesk-blueprint PackageKit-Qt5 appstream-qt
%endif

%if 0%{?fedora} >= 33
BuildRequires:  make qt5-qtbase-devel the-libs-blueprint-devel libtdesktopenvironment-blueprint-devel libthedesk-blueprint-devel PackageKit-Qt5-devel appstream-qt-devel
Requires:       qt5-qtbase the-libs-blueprint libtdesktopenvironment-blueprint libthedesk-blueprint PackageKit-Qt5 appstream-qt
%endif

%define debug_package %{nil}
%define _unpackaged_files_terminate_build 0

%description
Shopping Center

%prep
%setup

%build
qmake-qt5
make

%install
rm -rf $RPM_BUILD_ROOT
#%make_install
make install INSTALL_ROOT=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_bindir}/theshop
%{_libdir}/thedesk/plugins/*
%{_datadir}/applications/com.vicr123.theshop.desktop
%{_datadir}/icons/hicolor/scalable/apps/theshop.svg

%changelog
* Sun May  3 2020 Victor Tran
- 
