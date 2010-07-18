%define name slurm
%define version 0.2.3
%define release 1

Summary: A realtime network traffic statistic tool
Name: %{name}
Version: %{version}
Release: %{release}
Source0: %{name}-%{version}.tar.gz
License: GPL
Group: Applications/Multimedia
BuildRoot: %{_tmppath}/%{name}-buildroot
Requires: ncurses
BuildRequires: ncurses-devel
Packager: Ralf Ertzinger <ertzinger@skytale.net>
URL: http://www.wormulon.net/projects/slurm

%description
slurm started as a pppstatus port to FreeBSD. Features:

* realtime traffic statistics divided into incoming and outgoing
* optional combined view
* can monitor any kind of network interface (testers welcome!)
* shows detailed statistics about the interface.
* contact me if you need anything else. 

%prep
%setup

%build
%configure
%{__make}

%install
[ $RPM_BUILD_ROOT != "/" ] && rm -rf $RPM_BUILD_ROOT
%{__mkdir} -p $RPM_BUILD_ROOT

%{__install} -d %{buildroot}/%{_bindir}
%{__install} -d %{buildroot}/%{_mandir}/man1
%{__install} slurm %{buildroot}/%{_bindir}
%{__install} slurm.1 %{buildroot}/%{_mandir}/man1

%clean
[ $RPM_BUILD_ROOT != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc Changelog README KEYS TODO THANKS COPYRIGHT
%{_bindir}/slurm
%{_mandir}/man1/slurm.1.gz

%changelog
* Thu May 15 2003 Ralf Ertzinger <ertzinger@skytale.net>
- Initial RPM build

# end of file
