%define name	dsh
%define ver		0.25.2
%define rel		1

Summary			:	SSH Shell for connection to multiple server at one time
Name				:	%{name}
Version			:	%{ver}
Release			:	%{rel}
Copyright		:	GPL
Group				:	Applications/Internet
URL				:	http://www.netfort.gr.jp/~dancer/software/downloads/list.cgi
Source			:	%{name}-%{ver}.tar.gz
BuildRoot		:	/var/tmp/%{name}-buildroot
BuildRequires	:	libdshconfig-devel
Requires			:	libdshconfig

%description
Distributed shell. Runs command through rsh or ssh on a cluster of machines.
Requires libdshconfig to be already installed on the system.

%prep
%setup -q

%build
%configure
make

%install
rm -rf "$RPM_BUILD_ROOT"
make DESTDIR="$RPM_BUILD_ROOT" install

%files
%defattr(-,root,root)
%defattr(-,root,root)
%attr(0644,root,root) /usr/share/locale/ja/LC_MESSAGES/dsh.mo
%attr(0644,root,root) /usr/share/man/ja/man1/dsh.1.gz
%attr(0644,root,root) /usr/share/man/ja/man5/dsh.conf.5.gz
%attr(0644,root,root) /usr/share/man/man1/dsh.1.gz
%attr(0644,root,root) /usr/share/man/man5/dsh.conf.5.gz
%attr(0755,root,root) /usr/bin/dsh
%attr(0644,root,root) /etc/dsh.conf

%changelog
* Mon May 10 2004 Erik Wasser <ew@iquer.net>
- Initial spec-file
- Please don't hurt me.

