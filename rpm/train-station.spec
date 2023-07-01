Name:       train-station
Summary:    A train controller
Version:    0.2.2
Release:    1
License:    GPLv3+
URL:        https://github.com/dcaliste/train-station
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  cmake
BuildRequires:  extra-cmake-modules
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(KF5BluezQt)
BuildRequires:  desktop-file-utils

%description
A train controller application.

%prep
%setup -q -n %{name}-%{version}

%build
%cmake
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install

desktop-file-install --delete-original \
                     --dir %{buildroot}%{_datadir}/applications \
                     %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop

%changelog
* Sat Jul 1 2023 - Damien Caliste <dcaliste@free.fr> 0.2.1-1
- Fix cover layout
- Fix connection issues on unpowered adapters.

* Sat Jul 1 2023 - Damien Caliste <dcaliste@free.fr> 0.2.1-1
- Fix track label

* Fri Jun 30 2023 - Damien Caliste <dcaliste@free.fr> 0.2-1
- Add a cover page

* Thu Jun 22 2023 - Damien Caliste <dcaliste@free.fr> 0.0.1-1
- Initial package
