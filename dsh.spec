Summary: 	Run a single command on many hosts.
Name: 		dsh
Version: 	0.25.9
Release: 	2%{?disttag}
License: 	GPL
Group: 		Applications/Internet
URL: 		http://www.netfort.gr.jp/~dancer/software/dsh.html.en
Source0: 	http://www.netfort.gr.jp/~dancer/software/downloads/dsh-%{version}.tar.gz
BuildRoot: 	%{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
BuildRequires:	libdshconfig-devel


%description
Distributed shell. Runs command through rsh or ssh on a cluster of
machines.


%prep
%setup -q
%patch -p0 -b .hide


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
%makeinstall
%find_lang dsh


%clean
rm -rf $RPM_BUILD_ROOT


%files -f %{name}.lang
%defattr(-,root,root,-)
%doc COPYING README
%config(noreplace) %{_sysconfdir}/dsh.conf
%{_bindir}/dsh
%{_mandir}/man1/dsh.1*
%{_mandir}/man5/dsh.conf.5*
%{_mandir}/*/man1/dsh.1*
%{_mandir}/*/man5/dsh.conf.5*


%changelog
* Wed Jun 13 2007 Dams <anvil[AT]livna.org> - 0.25.8-2
- Updated to 0.25.8

* Tue Jan  2 2007 Dams <anvil[AT]livna.org> - 0.25.6-2
- Added patch to not print machine names

* Tue Feb 14 2006 Dams <anvil[AT]livna.org> - 0.25.6-1
- Updated to 0.25.6

* Tue Feb  1 2005 Dams <anvil[AT]livna.org> 0.25.4-2
- Fixed some typo

* Wed Jan 26 2005 Dams <anvil[AT]livna.org> 0.25.4-1
- Updated to 0.25.4

* Thu Jun  3 2004 Dams <anvil[AT]livna.org> 0:0.25.2-0.fdr.1
- Updated to 0.25.2
- More man pages

* Thu Jun  3 2004 Dams <anvil[AT]livna.org> 
- Initial build.

